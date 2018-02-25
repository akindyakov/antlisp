#include "parser.h"

#include <antlisp/src/util/string.h>

#include <iterator>


namespace AntLisp {

ParenthesesParser ParenthesesParser::openFromCodeStream(
    InCodeStream& codeStream
) {
    codeStream.skipSpaces();
    auto startLevel = codeStream.pCount();
    if (!codeStream.ignore()) {
        throw Error();
    }
    if (
        !(startLevel < codeStream.pCount())
    ) {
        throw Error() << "'";
    }
    return ParenthesesParser(
        codeStream,
        codeStream.pCount()
    );
}

ParenthesesParser ParenthesesParser::fromCodeStream(
    InCodeStream& codeStream
) {
    return ParenthesesParser(
        codeStream,
        codeStream.pCount()
    );
}

bool ParenthesesParser::isLocked() const {
    return level < codeStream.pCount();
}

bool ParenthesesParser::good() const {
    //**/ std::cerr << level << " == " << codeStream.pCount() << '\n';
    return (
        level == codeStream.pCount()
        && codeStream.good()
    );
}

bool ParenthesesParser::nextToken(
    std::string& token
) {
    if (this->good()) {
        if (codeStream.nextToken(token)) {
            return true;
        }
        if (codeStream.good()) {
            codeStream.ignore();
        }
        return false;
    }
    return false;
}

std::string ParenthesesParser::nextToken() {
    auto token = std::string{};
    if (!this->nextToken(token)) {
        throw Error() << "Unexpected end of stream";
    }
    return token;
}

ParenthesesParser ParenthesesParser::nextParser() {
    return ParenthesesParser(
        codeStream,
        codeStream.pCount()
    );
}

ParenthesesParser::ParenthesesParser(
    InCodeStream& reader_
    , int level_
)
    : codeStream(reader_)
    , level(level_)
{
}

/**
* function call
* function definition
* let
* cond
* lambda
*/
namespace {

class ConstructionParser {
public:
    explicit ConstructionParser (
        const Namespace& global
    )
    {
        // may be put global here as closures?
        definitionStack.push_back(
            std::make_shared<LambdaFunction>(
                NativeFunction(
                    std::make_shared<NativeFunctionDefinition>(),
                    0, global
                ),
                std::vector<TVarName>{} // global arg names - should be empty
            )
        );
    }

    ConstructionParser& fromCodeStream(
        InCodeStream& inStream
    ) {
        this->next(
            ParenthesesParser::openFromCodeStream(inStream)
        );
        return *this;
    }

    LambdaFunctionPtr finish() {
        if (definitionStack.size() != 1) {
            throw Error()
                << __FILE__ << ":" << __LINE__
                << " Definition stack should has size 1";
        }
        return std::move(
            definitionStack.back()
        );
    }

    class Error
        : public Exception
    {
    };

private:
    void next(
        ParenthesesParser pParser
    ) {
        auto token = std::string{};
        if (pParser.nextToken(token)) {
            //**/ std::cerr << "next " << Str::Quotes(token) << '\n';
            if ("defun" == token) {
                functionDef(std::move(pParser));
            } else if ("lambda" == token) {
                lambdaDef(std::move(pParser));
            } else if ("let" == token) {
                letDef(std::move(pParser));
            } else if ("cond" == token) {
                condDef(std::move(pParser));
            } else {
                tokenDef(token);
                callDef(std::move(pParser));
            }
        } else {
            //**/ std::cerr << "next go deeper\n";
            if (pParser.isLocked()) {
                next(pParser.nextParser());
                callDef(std::move(pParser));
            } else {
                //**/ std::cerr << "p\n";
                throw Error();
            }
        }
    }

    void functionDef(
        ParenthesesParser pParser
    ) {
        auto fname = std::string{};
        if (!pParser.nextToken(fname)) {
            throw Error() << __FILE__ << ":" << __LINE__ << " there is suppose to be function name.";
        }
        lambdaDef(std::move(pParser));
        auto core = definitionStack.back()->core();
        auto pos = core->names.size();
        core->names.push_back(fname);
        core->operations.emplace_back(
            NativeFunctionDefinition::SetLocal,
            pos
        );
    }

    void lambdaDef(
        ParenthesesParser pParser
    ) {
        auto argNames = ArgNames{};
        auto token = std::string{};
        if (pParser.nextToken(token)) {
            throw Error() << __FILE__ << ":" << __LINE__ << " wtf? " << token;
        }
        {
            auto argParser = pParser.nextParser();
            while (argParser.nextToken(token)) {
                //**/ std::cerr << "read arg name: " << token << '\n';
                argNames.push_back(token);
            }
            if (argParser.good()) {
                throw Error() << "Code stream of lambda arguments is suppose to be exhausted";
            }
        }
        auto core = definitionStack.back()->core();
        auto argnum = argNames.size();
        auto newLambda = std::make_shared<LambdaFunction>(
            NativeFunction(
                std::make_shared<NativeFunctionDefinition>(
                    std::move(argNames)
                ),
                argnum,
                Namespace{}
            ),
            ArgNames{}
        );
        core->consts.push_back(
            Cell::function(
                newLambda
            )
        );
        core->operations.emplace_back(
            NativeFunctionDefinition::GetConst,
            core->consts.size() - 1
        );
        definitionStack.push_back(newLambda);
        if (pParser.nextToken(token)) {
            throw Error() << __FILE__ << ":" << __LINE__ << " wtf?";
        }
        // read the body of lambda
        next(pParser.nextParser());
        // vvv FIXME: create flush or finish methods for this place
        pParser.nextToken(token);
        // ^^^
        definitionStack.pop_back();
        argnum = newLambda->names.size();
        core->operations.emplace_back(
            NativeFunctionDefinition::RunFunction,
            argnum
        );
    }

    void letDef(
        ParenthesesParser pParser
    ) {
        // TODO
        auto token = std::string{};
        {
            auto argListParser = pParser.nextParser();
            while (true) {
                argListParser.nextToken(token);
                if (!argListParser.isLocked()) {
                    break;
                }
                auto argParser = argListParser.nextParser();
                if (!argParser.nextToken(token)) {
                    throw Error() << __FILE__ << ":" << __LINE__;
                }
                if (!argParser.nextToken(token)) {
                    throw Error() << __FILE__ << ":" << __LINE__;
                }
            }
        }
    }

    void condDef(
        ParenthesesParser pParser
    ) {
        // TODO
    }

    void callDef(
        ParenthesesParser pParser
    ) {
        //**/ std::cerr << "call def\n";
        auto argCount = std::size_t{0};
        auto token = std::string{};
        while (pParser.good()) {
            if (pParser.nextToken(token)) {
                //**/ std::cerr << "token var (" << argCount << ") " << Str::Quotes(token) << "\n";
                tokenDef(token);
                ++argCount;
            } else if (pParser.isLocked()) {
                //**/ std::cerr << "other var\n";
                next(pParser.nextParser());
                ++argCount;
            }
        }
        //**/ std::cerr << "argnum: " << argCount << "\n";
        auto core = definitionStack.back()->core();
        core->operations.emplace_back(
            NativeFunctionDefinition::RunFunction,
            argCount
        );
    }

    void tokenDef(
        const std::string& token
    ) {
        auto cellOpt = tryFromString(token);
        if (cellOpt) {
            //**/ std::cerr << "add const (" << definitionStack.size() << ") " << cellOpt->toString() << '\n';
            auto core = definitionStack.back()->core();
            auto pos = core->consts.size();
            core->consts.push_back(
                std::move(cellOpt.get())
            );
            core->operations.emplace_back(
                NativeFunctionDefinition::GetConst,
                pos
            );
        } else {
            takeVarByName(token);
        }
    }

    void takeVarByName(
        const std::string& varName
    ) {
        auto to = definitionStack.rbegin();
        while (
            to != definitionStack.rend()
            && not to->get()->hasName(varName)
        ) {
            ++to;
        }
        if (to == definitionStack.rend()) {
            throw Error() << "There is no such variable " << Str::Quotes(varName);
        }
        for (
            auto it = definitionStack.rbegin(); it != to; ++it
        ) {
            it->get()->names.push_back(varName);
        }
        for (
            auto it = definitionStack.rbegin(); it <= to; ++it
        ) {
            auto core = it->get()->core();
            auto pos = core->names.size();
            core->names.push_back(varName);
            core->addStep(
                NativeFunctionDefinition::GetLocal,
                pos
            );
        }
    }

private:
    std::vector<LambdaFunctionPtr> definitionStack;
};

}  // namespace

LambdaFunctionPtr parseCode(
    std::istream& in
    , const Namespace& global
) {
    auto codeStream = InCodeStream(in);
    return ConstructionParser(global).fromCodeStream(
        codeStream
    ).finish();
}

}  // namespace AntLisp
