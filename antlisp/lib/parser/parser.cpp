#include "parser.h"

#include "cell_in_string.h"
#include "parentheses_parser.h"

#include <antlisp/lib/util/string.h>

#include <iterator>


namespace AntLisp {

namespace {

class ConstructionParser {
public:
    explicit ConstructionParser (
        Namespace global
    )
    {
        definitionStack.push_back(
            std::make_shared<LambdaFunction>(
                NativeFunction(
                    std::make_shared<NativeTape>(),
                    0, std::move(global), "this"
                ),
                std::vector<TVarName>{} // global arg names - should be empty
            )
        );
    }

    ConstructionParser& fromCodeStream(
        InCodeStream& inStream
    ) {
        auto topLevel = ParenthesesParser::fromCodeStream(inStream);
        this->prognDef(topLevel);
        topLevel.close();
        return *this;
    }

    NativeFunction finish() {
        if (definitionStack.size() != 1) {
            throw ParseError()
                << __FILE__ << ":" << __LINE__
                << " Definition stack should have size 1";
        }
        return std::move(
            definitionStack.back()->nativeFn
        );
    }

private:
    bool parenthesesExpression(
        ParenthesesParser& pParser
    ) {
        auto nextParser = pParser.nextParser();
        if (not nextParser) {
            return false;
        }
        auto token = std::string{};
        if (nextParser->nextToken(token)) {
            // TODO(akindyakov): make different language mechanisms availability configurable
            if ("defun" == token) {
                functionDef(nextParser.get());
            } else if ("lambda" == token) {
                lambdaDef(nextParser.get(), "this");
            } else if ("let" == token) {
                letDef(nextParser.get());
            } else if ("set" == token) {
                setDef(nextParser.get());
            } else if ("cond" == token) {
                condDef(nextParser.get());
            } else if ("progn" == token) {
                prognDef(nextParser.get());
            } else {
                tokenDef(token);
                callDef(nextParser.get());
            }
        } else {
            if (nextParser->isEnd()) {
                return false;
            }
            if (not parenthesesExpression(nextParser.get())) {
                return false;
            }
            callDef(nextParser.get());
        }
        nextParser->close();
        return true;
    }

    bool expression(
        ParenthesesParser& pParser
    ) {
        auto token = std::string{};
        if (pParser.nextToken(token)) {
            tokenDef(token);
        } else {
            return parenthesesExpression(pParser);
        }
        return true;
    }

    void prognDef(
        ParenthesesParser& pParser
    ) {
        auto core = definitionStack.back()->core();
        while (expression(pParser)) {
            core->addStep(
                NativeTape::LocalStackRewind,
                1
            );
        }
        core->operations.pop_back();
    }

    void functionDef(
        ParenthesesParser& pParser
    ) {
        auto fname = std::string{};
        if (!pParser.nextToken(fname)) {
            throw SyntaxError() << pParser.getStat().toString() << " there is supposed to be function name.";
        }
        auto core = definitionStack.back()->core();
        auto functionNamePos = core->names.size();
        core->names.push_back(fname);
        // read and put lambda to stack
        lambdaDef(pParser, fname);
        // add defined lambda to local namespace
        core->addStep(
            NativeTape::SetLocal,
            functionNamePos
        );
    }

    void lambdaDef(
        ParenthesesParser& pParser
        , const std::string& selfName
    ) {
        auto argNames = ArgNames{};
        auto token = std::string{};
        {
            auto argParser = pParser.nextParser();
            if (not argParser) {
                throw SyntaxError()
                    << pParser.getStat().toString()
                    << " here is supposed to be argument list";
            }
            while (argParser->nextToken(token)) {
                argNames.push_back(token);
            }
            argParser->close();
        }
        auto core = definitionStack.back()->core();
        auto argnum = argNames.size();
        auto newLambda = std::make_shared<LambdaFunction>(
            NativeFunction(
                std::make_shared<NativeTape>(
                    std::move(argNames)
                ),
                argnum,
                Namespace{},
                selfName
            ),
            ArgNames{}
        );
        core->consts.push_back(
            Cell::function(
                newLambda
            )
        );
        core->addStep(
            NativeTape::GetConst,
            core->consts.size() - 1
        );
        definitionStack.push_back(newLambda);
        if (not expression(pParser)) {
            throw SyntaxError() << pParser.getStat().toString() << ", there is suppose to be lambda body";
        }
        pParser.close();
        definitionStack.back()->core()->ApplyTailRecursionOptimization();

        definitionStack.pop_back();
        argnum = newLambda->names.size();
        core->addStep(
            NativeTape::RunFunction,
            argnum
        );
    }

    void letDef(
        ParenthesesParser& /*pParser*/
    ) {
        // TODO(akindyakov): implement 'let' language mechanism
        throw ParseError() << "let construction is not implemented";
    }

    void setDef(
        ParenthesesParser& setParser
    ) {
        auto name = std::string{};
        if (!setParser.nextToken(name)) {
            throw SyntaxError() << setParser.getStat().toString() << ", there is suppose to be name.";
        }
        this->expression(setParser);
        auto core = definitionStack.back()->core();
        auto pos = core->names.size();
        core->names.push_back(name);
        core->addStep(
            NativeTape::SetLocal,
            pos
        );
    }

    void condDef(
        ParenthesesParser& condParser
    ) {
        auto core = definitionStack.back()->core();
        auto endMark = getMarkUid();
        while (auto branchParser = condParser.nextParser()) {
            auto token = std::string{};
            expression(branchParser.value());
            auto branchEndMark = getMarkUid();
            core->addStep(
                NativeTape::SkipIfNil,
                branchEndMark
            );

            expression(branchParser.value());
            core->addStep(
                NativeTape::Skip,
                endMark
            );
            core->addStep(
                NativeTape::GuardMark,
                branchEndMark
            );
            branchParser->close();
        }
        core->addStep(
            NativeTape::GuardMark,
            endMark
        );
    }

    void callDef(
        ParenthesesParser& pParser
    ) {
        auto argCount = std::size_t{0};
        auto token = std::string{};
        while (expression(pParser)) {
            ++argCount;
        }
        auto core = definitionStack.back()->core();
        core->addStep(
            NativeTape::RunFunction,
            argCount
        );
    }

    void tokenDef(
        const std::string& token
    ) {
        auto cellOpt = tryCellFromString(token);
        if (cellOpt) {
            auto core = definitionStack.back()->core();
            auto pos = core->consts.size();
            core->consts.push_back(
                std::move(cellOpt.get())
            );
            core->addStep(
                NativeTape::GetConst,
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
            throw NameError()
                << "There is no such variable " << Str::Quotes(varName);
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
                NativeTape::GetLocal,
                pos
            );
        }
    }

    NativeTape::Step::Operand
    getMarkUid() {
        ++markCounter;
        const auto bitSize = 8 * sizeof(NativeTape::Step::Operand);
        return (
            (this->definitionStack.size() << (bitSize / 2)) ^ markCounter
        );
    }

private:
    std::vector<LambdaFunctionPtr> definitionStack;
    NativeTape::Step::Operand markCounter = 0;
};

}  // namespace

NativeFunction parseCode(
    std::istream& in
    , Namespace global
) {
    auto codeStream = InCodeStream(in);
    return ConstructionParser(std::move(global)).fromCodeStream(
        codeStream
    ).finish();
}

}  // namespace AntLisp
