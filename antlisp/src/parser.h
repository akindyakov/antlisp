#pragma once

#include "exception.h"

#include <antlisp/src/stack_machine.h>


namespace AntLisp {

using Char = char;

class InCodeStream
{
public:
    explicit InCodeStream(
        std::istream& in
    );

    class Error
        : public Exception
    {
    };

    bool good() const;

    int pCount() const;

    Char peek() const;
    bool peek(Char& ch) const;

    bool next(Char& ch);

    bool ignore();

    bool nextToken(
        std::string& token
    );
    std::string nextToken();

    void skipSpaces();

    enum Parentheses
        : Char
    {
        CHAR_OPEN = '(',
        CHAR_CLOSE = ')',
    };

    static int getParenthesesNumber(Char ch);

private:
    std::istream& istream;
    int parenthesesCounter = 0;
};

class ParenthesesParser
{
public:
    class Error
        : public Exception
    {
    };

    static ParenthesesParser openFromCodeStream(
        InCodeStream& codeStream
    );
    static ParenthesesParser fromCodeStream(
        InCodeStream& codeStream
    );

    bool isLocked() const;

    bool good() const;

    bool nextToken(
        std::string& token
    );

    std::string nextToken();

    ParenthesesParser nextParser();

private:
    explicit ParenthesesParser(
        InCodeStream& reader_
        , int level_
    );

private:
    InCodeStream& codeStream;
    int level = 0;
};

FunctionDefinition parseCode(
    std::istream& in
    , Namespace& global
);

class ConstructionParser {
public:
    explicit ConstructionParser (
        Namespace& global
    )
        : global_(global)
    {
    }

    void next(
        ParenthesesParser pParser
        , FunctionDefinition& fdef
    ) {
        auto token = std::string{};
        if (!pParser.nextToken(token)) {
            if ("define" == token) {
                functionDef(std::move(pParser), fdef);
            } else if ("lambda" == token) {
                lambdaDef(std::move(pParser), fdef);
            } else if ("let" == token) {
                letDef(std::move(pParser), fdef);
            } else if ("cond" == token) {
                condDef(std::move(pParser), fdef);
            } else {
                fdef.getGlobalName(token);
                callDef(std::move(pParser), fdef);
            }
        } else {
            if (pParser.good()) {
                next(pParser.nextParser(), fdef);
                callDef(std::move(pParser), fdef);
            } else {
                throw Error();
            }
        }
    }

    void bodyDef(
        ParenthesesParser pParser
        , FunctionDefinition& fdef
    ) {
        // TODO
    }

    void functionDef(
        ParenthesesParser pParser
        , FunctionDefinition& fdef
    ) {
        // TODO
    }

    void lambdaDef(
        ParenthesesParser pParser
        , FunctionDefinition& fdef
    ) {
        // TODO
    }

    void letDef(
        ParenthesesParser pParser
        , FunctionDefinition& fdef
    ) {
        // TODO
    }

    void condDef(
        ParenthesesParser pParser
        , FunctionDefinition& fdef
    ) {
        // TODO
    }

    void callDef(
        ParenthesesParser pParser
        , FunctionDefinition& fdef
    ) {
        auto argCount = std::size_t{0};
        auto token = std::string{};
        while (pParser.good()) {
            if (pParser.nextToken(token)) {
                tokenDef(token, fdef);
            } else if (pParser.isLocked()) {
                next(pParser.nextParser(), fdef);
            }
        }
        fdef.operations.emplace_back(
            FunctionDefinition::RunFunction,
            argCount
        );
    }

    void tokenDef(
        const std::string& token
        , FunctionDefinition& fdef
    ) {
    }

    void finish() {
        for (auto& value : globalPatch_) {
            global_[value.first] = std::move(value.second);
        }
        globalPatch_.clear();
    }

private:
    Namespace& global_;
    Namespace globalPatch_;
};

}  // namespace AntLisp
