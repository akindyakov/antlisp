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
            // skip the last parenthesis to close parser
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

bool ParenthesesParser::check() {
    if (this->good()) {
        auto token = std::string{};
        if (this->nextToken(token)) {
            throw Error() << __FILE__ << ":" << __LINE__
                << " wtf, there is token " << Str::Quotes(token);
        }
        return this->good();
    }
    return false;
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
        auto topLevel = ParenthesesParser::openFromCodeStream(inStream);
        this->next(topLevel);
        return *this;
    }

    NativeFunction finish() {
        if (definitionStack.size() != 1) {
            throw Error()
                << __FILE__ << ":" << __LINE__
                << " Definition stack should has size 1";
        }
        return std::move(
            definitionStack.back()->nativeFn
        );
    }

    class Error
        : public Exception
    {
    };

private:
    void next(
        ParenthesesParser& pParser
    ) {
        //**/ std::cerr << "next\n";
        auto token = std::string{};
        if (pParser.nextToken(token)) {
            //**/ std::cerr << "next " << Str::Quotes(token) << '\n';
            if ("defun" == token) {
                functionDef(pParser);
            } else if ("lambda" == token) {
                lambdaDef(pParser);
            } else if ("let" == token) {
                letDef(pParser);
            } else if ("cond" == token) {
                condDef(pParser);
            } else {
                tokenDef(token);
                callDef(pParser);
            }
        } else {
            //**/ std::cerr << "next go deeper\n";
            if (pParser.isLocked()) {
                {
                    auto nextParser = pParser.nextParser();
                    next(nextParser);
                }
                callDef(pParser);
            } else {
                //**/ std::cerr << "p\n";
                throw Error();
            }
        }
    }

    void functionDef(
        ParenthesesParser& pParser
    ) {
        auto fname = std::string{};
        if (!pParser.nextToken(fname)) {
            throw Error() << __FILE__ << ":" << __LINE__ << " there is suppose to be function name.";
        }
        lambdaDef(pParser);
        auto core = definitionStack.back()->core();
        auto pos = core->names.size();
        core->names.push_back(fname);
        core->operations.emplace_back(
            NativeFunctionDefinition::SetLocal,
            pos
        );
    }

    void lambdaDef(
        ParenthesesParser& pParser
    ) {
        auto argNames = ArgNames{};
        auto token = std::string{};
        pParser.check();
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
        pParser.check();
        {
            // read the body of lambda
            auto bodyParser = pParser.nextParser();
            next(bodyParser);
        }
        pParser.check();
        definitionStack.pop_back();
        argnum = newLambda->names.size();
        core->operations.emplace_back(
            NativeFunctionDefinition::RunFunction,
            argnum
        );
    }

    void letDef(
        ParenthesesParser& pParser
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
        ParenthesesParser& condParser
    ) {
        auto core = definitionStack.back()->core();
        auto endMark = getMarkUid();
        condParser.check();
        while (condParser.isLocked()) {
            auto branchParser = condParser.nextParser();
            auto token = std::string{};
            if (branchParser.nextToken(token)) {
                tokenDef(token);
            } else {
                auto branchGuardParser = condParser.nextParser();
                next(branchGuardParser);
            }
            auto branchEndMark = getMarkUid();
            core->operations.emplace_back(
                NativeFunctionDefinition::SkipIfNil,
                branchEndMark
            );
            branchParser.check();

            if (branchParser.nextToken(token)) {
                tokenDef(token);
            } else {
                auto branchBodyParser = branchParser.nextParser();
                next(branchBodyParser);
            }
            core->operations.emplace_back(
                NativeFunctionDefinition::Skip,
                endMark
            );
            core->operations.emplace_back(
                NativeFunctionDefinition::GuardMark,
                branchEndMark
            );
            // here!
            branchParser.check();
            condParser.check();
        }
        core->operations.emplace_back(
            NativeFunctionDefinition::GuardMark,
            endMark
        );
    }

    void callDef(
        ParenthesesParser& pParser
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
                auto nextParser = pParser.nextParser();
                next(nextParser);
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

    NativeFunctionDefinition::Step::Operand
    getMarkUid() {
        ++markCounter;
        const auto bitSize = 8 * sizeof(NativeFunctionDefinition::Step::Operand);
        return (
            (this->definitionStack.size() << (bitSize / 2))
            | markCounter
        );
    }

private:
    std::vector<LambdaFunctionPtr> definitionStack;
    NativeFunctionDefinition::Step::Operand markCounter = 0;
};

}  // namespace

NativeFunction parseCode(
    std::istream& in
    , const Namespace& global
) {
    auto codeStream = InCodeStream(in);
    return ConstructionParser(global).fromCodeStream(
        codeStream
    ).finish();
}

}  // namespace AntLisp
