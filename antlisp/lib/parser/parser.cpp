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
        const Namespace& global
    )
    {
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
        auto topLevel = ParenthesesParser::fromCodeStream(inStream);
        this->expression(topLevel);
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
        //**/ std::cerr << "parentheses next\n";
        auto nextParser = pParser.nextParser();
        auto token = std::string{};
        if (nextParser.nextToken(token)) {
            //**/ std::cerr << "next parser " << Str::Quotes(token) << '\n';
            if ("defun" == token) {
                functionDef(nextParser);
            } else if ("lambda" == token) {
                lambdaDef(nextParser);
            } else if ("let" == token) {
                letDef(nextParser);
            } else if ("cond" == token) {
                condDef(nextParser);
            } else if ("progn" == token) {
                prognDef(nextParser);
            } else {
                //**/ std::cerr << "call (\n";
                tokenDef(token);
                callDef(nextParser);
                //**/ std::cerr << ")\n";
            }
        } else {
            //**/ std::cerr << "next -> next\n";
            if (not parenthesesExpression(nextParser)) {
                return false;
            }
            callDef(nextParser);
        }
        return true;
    }

    bool expression(
        ParenthesesParser& pParser
    ) {
        //**/ std::cerr << "next\n";
        auto token = std::string{};
        if (pParser.nextToken(token)) {
            //**/ std::cerr << "next token " << Str::Quotes(token) << "\n";
            tokenDef(token);
        } else {
            if (not pParser.good()) {
                //**/ std::cerr << "next -> false\n";
                return false;
            }
            return parenthesesExpression(pParser);
        }
        return true;
    }

    void prognDef(
        ParenthesesParser& pParser
    ) {
        auto core = definitionStack.back()->core();
        std::cerr << "progn parse start\n";
        while (expression(pParser)) {
            std::cerr << "progn line\n";
            core->addStep(
                NativeFunctionDefinition::LocalStackRewind,
                1
            );
        }
        core->operations.pop_back();
        std::cerr << "progn pop back\n";
        for (const auto& op : core->operations) {
            std::cerr << int(op.operation) << ": " << op.position << "\n";
        }
    }

    void functionDef(
        ParenthesesParser& pParser
    ) {
        auto fname = std::string{};
        if (!pParser.nextToken(fname)) {
            throw SyntaxError() << __FILE__ << ":" << __LINE__ << " there is suppose to be function name.";
        }
        lambdaDef(pParser);
        auto core = definitionStack.back()->core();
        auto pos = core->names.size();
        core->names.push_back(fname);
        core->addStep(
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
                throw SyntaxError() << "Code stream of lambda arguments is suppose to be exhausted";
            }
        }
        //**/ std::cerr << "end of args list\n";
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
        core->addStep(
            NativeFunctionDefinition::GetConst,
            core->consts.size() - 1
        );
        definitionStack.push_back(newLambda);
        expression(pParser);
        pParser.check();
        definitionStack.pop_back();
        argnum = newLambda->names.size();
        core->addStep(
            NativeFunctionDefinition::RunFunction,
            argnum
        );
        //**/ std::cerr << "end of lambda definition\n";
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
                    throw ParseError() << __FILE__ << ":" << __LINE__;
                }
                if (!argParser.nextToken(token)) {
                    throw ParseError() << __FILE__ << ":" << __LINE__;
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
            //**/ std::cerr << "create branchParser\n";
            auto branchParser = condParser.nextParser();
            auto token = std::string{};
            expression(branchParser);
            auto branchEndMark = getMarkUid();
            core->addStep(
                NativeFunctionDefinition::SkipIfNil,
                branchEndMark
            );
            //**/ std::cerr << "branchParser.check()\n";

            expression(branchParser);
            core->addStep(
                NativeFunctionDefinition::Skip,
                endMark
            );
            core->addStep(
                NativeFunctionDefinition::GuardMark,
                branchEndMark
            );
            // here!
            branchParser.check();
            condParser.check();
        }
        core->addStep(
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
        while (expression(pParser)) {
            ++argCount;
        }
        //**/ std::cerr << "argnum: " << argCount << "\n";
        auto core = definitionStack.back()->core();
        core->addStep(
            NativeFunctionDefinition::RunFunction,
            argCount
        );
    }

    void tokenDef(
        const std::string& token
    ) {
        auto cellOpt = tryCellFromString(token);
        if (cellOpt) {
            //**/ std::cerr << "add const (" << definitionStack.size() << ") " << cellOpt->toString() << '\n';
            auto core = definitionStack.back()->core();
            auto pos = core->consts.size();
            core->consts.push_back(
                std::move(cellOpt.get())
            );
            core->addStep(
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
            (this->definitionStack.size() << (bitSize / 2)) ^ markCounter
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
