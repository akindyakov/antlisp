#pragma once

#include "exception.h"

#include <antlisp/src/stack_machine.h>


namespace AntLisp {

/**
* function call
* function definition
* let
* cond
* lambda
*/

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
    , GlobalFrame global
);

}  // namespace AntLisp
