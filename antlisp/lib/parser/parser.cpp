#include "parser.h"

#include <antlisp/lib/parser/cell/cell_in_string.h>
#include <antlisp/lib/parser/parentheses/parentheses_parser.h>

#include <antlisp/lib/util/string.h>

#include <iterator>


namespace AntLisp {

ParserOptions::ParserOptions()
{
    this->set<Keywords::AllKeywords>();
}

namespace {

class ConstructionParser {
public:
    explicit ConstructionParser (
        Namespace global
        , const ParserOptions& opts
    )
        : options_(opts)
    {
        definitionStack.push_back(
            std::make_shared<LambdaFunction>(
                NativeFunction(
                    std::make_shared<NativeTape>(),
                    0, std::move(global), "this"
                ),
                std::vector<VarName>{} // global arg names - should be empty
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
                if (not options_.test<Keywords::Defun>()) {
                    throw ParseError() << "Forbidden keyword [defun]";
                }
                functionDef(nextParser.get());
            } else if ("lambda" == token) {
                if (not options_.test<Keywords::Lambda>()) {
                    throw ParseError() << "Forbidden keyword [lambda]";
                }
                lambdaDef(nextParser.get(), "this");
            } else if ("let" == token) {
                if (not options_.test<Keywords::Let>()) {
                    throw ParseError() << "Forbidden keyword [let]";
                }
                letDef(nextParser.get());
            } else if ("set" == token) {
                if (not options_.test<Keywords::Set>()) {
                    throw ParseError() << "Forbidden keyword [set]";
                }
                setDef(nextParser.get());
            } else if ("cond" == token) {
                if (not options_.test<Keywords::Cond>()) {
                    throw ParseError() << "Forbidden keyword [cond]";
                }
                condDef(nextParser.get());
            } else if ("progn" == token) {
                if (not options_.test<Keywords::Progn>()) {
                    throw ParseError() << "Forbidden keyword [progn]";
                }
                prognDef(nextParser.get());
            } else if ("load" == token) {
                if (not options_.test<Keywords::Load>()) {
                    throw ParseError() << "Forbidden keyword [load]";
                }
                loadDef(nextParser.get());
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

    void loadDef(
        ParenthesesParser& pParser
    ) {
        takeVarByName("load");  // "load" should be defined as a function
        auto name = std::string{};
        if (!pParser.nextToken(name)) {
            throw SyntaxError()
                << pParser.getStat().toString()
                << " there is supposed to be prefix for the loading namespace."
            ;
        }
        auto core = definitionStack.back()->core();
        {
            auto pos = core->consts.size();
            core->consts.push_back(Cell::ext<StringType>(name));
            core->addStep(
                NativeTape::GetConst,
                pos
            );
        }
        auto argCount = std::size_t{1};
        if (expression(pParser)) {  // optional
            ++argCount;
        }
        auto something = std::string{};
        if (pParser.nextToken(something)) {
            throw SyntaxError() << pParser.getStat().toString() << " unexpected tocken " << Str::Quotes(something);
        }
        core->addStep(
            NativeTape::RunFunction,
            argCount
        );
        auto pos = core->names.size();
        core->names.push_back(name);
        core->addStep(
            NativeTape::SetLocal,
            pos
        );
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
        auto const prefix = VarNamePrefix(varName);
        auto const prefixStr = prefix.firstName();
        auto fromIt = definitionStack.rbegin();
        while (
            fromIt != definitionStack.rend()
            && not fromIt->get()->hasName(prefixStr)
        ) {
            ++fromIt;
        }
        if (fromIt == definitionStack.rend()) {
            throw NameError()
                << "There is no such variable "
                << Str::Quotes(prefix.fullName());
        }
        {
            auto caller = definitionStack.rbegin();
            if (caller != fromIt) {
                caller->get()->names.push_back(prefixStr);
            }
            auto core = caller->get()->core();
            auto pos = core->names.size();
            core->names.push_back(prefix.fullName());
            core->addStep(
                NativeTape::GetLocal,
                pos
            );
        }
        for (
            auto it = definitionStack.rbegin() + 1; it <= fromIt; ++it
        ) {
            if (it != fromIt) {
                it->get()->names.push_back(prefixStr);
            }
            auto core = it->get()->core();
            auto pos = core->names.size();
            core->names.push_back(prefixStr);
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
    const ParserOptions& options_;
    std::vector<LambdaFunctionPtr> definitionStack;
    NativeTape::Step::Operand markCounter = 0;
};

}  // namespace

NativeFunction parseCode(
    std::istream& in
    , Namespace global
    , const ParserOptions& opts
) {
    auto codeStream = InCodeStream(in);
    return ConstructionParser(std::move(global), opts).fromCodeStream(
        codeStream
    ).finish();
}

}  // namespace AntLisp
