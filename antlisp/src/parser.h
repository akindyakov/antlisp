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
    , GlobalFrame& global
);

class ConstructionParser {
public:
    explicit ConstructionParser (
        ParenthesesParser& parentStream
        , GlobalFrame& global
    )
        : parentStream_(parentStream)
        , global_(global)
    {
    }

    void choose(
        const std::string& token
        , FunctionDefinition& fdef
    ) {
        if ("function" == token) {
            functionDef(fdef);
        } else if ("lambda" == token) {
            lambdaDef(fdef);
        } else if ("let" == token) {
            letDef(fdef);
        } else if ("cond" == token) {
            condDef(fdef);
        } else {
            callDef(token, fdef);
        }
    }

    void bodyDef(
        FunctionDefinition& fdef
    ) {
    }

    void functionDef(
        FunctionDefinition& fdef
    ) {
    }

    void lambdaDef(
        FunctionDefinition& fdef
    ) {
    }

    void letDef(
        FunctionDefinition& fdef
    ) {
    }

    void condDef(
        FunctionDefinition& fdef
    ) {
    }

    void callDef(
        const std::string& token
        , FunctionDefinition& fdef
    ) {
        fdef.operations.emplace_back(
            FunctionDefinition::GetGlobal,
            fdef.addName(token)
        );
        auto argCount = std::size_t{0};
    }

    void finish() {
        for (auto& value : globalPatch_) {
            global_[value.first] = std::move(value.second);
        }
        globalPatch_.clear();
    }

private:
    ParenthesesParser& parentStream_;
    GlobalFrame& global_;
    GlobalFrame globalPatch_;
};

}  // namespace AntLisp
