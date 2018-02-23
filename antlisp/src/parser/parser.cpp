#include "code_stream.h"


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
        Namespace& global
    )
        : global_(global)
    {
        definitionStack.push_back(
            std::make_shared<FunctionDefinition>()
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

    FunctionDefinitionPtr finish() {
        for (auto& value : globalPatch_) {
            global_[value.first] = std::move(value.second);
        }
        if (definitionStack.size() != 1) {
            throw Error() << __FILE__ << ":" << __LINE__ << " Definition stack should has size 1";
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
            if ("define" == token) {
                functionDef(std::move(pParser));
            } else if ("lambda" == token) {
                lambdaDef(std::move(pParser));
            } else if ("let" == token) {
                letDef(std::move(pParser));
            } else if ("cond" == token) {
                condDef(std::move(pParser));
            } else {
                auto& fdef = definitionStack.back();
                fdef->getGlobalName(token);
                callDef(std::move(pParser));
            }
        } else {
            if (pParser.good()) {
                next(pParser.nextParser());
                callDef(std::move(pParser));
            } else {
                throw Error();
            }
        }
    }

    void bodyDef(
        ParenthesesParser pParser
    ) {
        // TODO
    }

    void functionDef(
        ParenthesesParser pParser
    ) {
        // TODO
    }

    void lambdaDef(
        ParenthesesParser pParser
    ) {
        // TODO
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
        auto argCount = std::size_t{0};
        auto token = std::string{};
        while (pParser.good()) {
            if (pParser.nextToken(token)) {
                tokenDef(token);
                ++argCount;
            } else if (pParser.isLocked()) {
                next(pParser.nextParser());
                ++argCount;
            }
        }
        auto& fdef = definitionStack.back();
        fdef->operations.emplace_back(
            FunctionDefinition::RunFunction,
            argCount
        );
    }

    void tokenDef(
        const std::string& token
    ) {
        auto& fdef = definitionStack.back();
        auto cellOpt = tryFromString(token);
        if (cellOpt) {
            auto pos = fdef->consts.size();
            fdef->consts.push_back(
                std::move(cellOpt.get())
            );
            fdef->operations.emplace_back(
                FunctionDefinition::GetConst,
                pos
            );
        } else {
            takeVarByName(token);
        }
    }

    void takeVarByName(
        const std::string& varName
    ) {
        auto& fdef = definitionStack.back();
        auto pos = fdef->names.size();
        fdef->names.push_back(varName);
        fdef->operations.emplace_back(
            FunctionDefinition::GetLocal,
            pos
        );
    }

private:
    Namespace& global_;
    Namespace globalPatch_;
    std::vector<FunctionDefinitionPtr> definitionStack;
};

}  // namespace

FunctionDefinitionPtr parseCode(
    std::istream& in
    , Namespace& global
) {
    auto codeStream = InCodeStream(in);
    return ConstructionParser(global).fromCodeStream(
        codeStream
    ).finish();
}

}  // namespace AntLisp
