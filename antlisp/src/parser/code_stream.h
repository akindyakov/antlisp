#pragma once

#include <antlisp/src/util/exception.h>

#include <antlisp/src/function/stack_machine.h>


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

}  // namespace AntLisp
