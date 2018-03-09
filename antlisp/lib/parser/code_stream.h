#pragma once

#include <antlisp/lib/util/exception.h>

#include <antlisp/lib/function/stack_machine.h>


namespace AntLisp {

using Char = char;

struct CodeStat {
    int parentheses = 0;
    std::size_t lines = 1;
    std::size_t characters = 0;
};

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

    const CodeStat& getStat() const;

    static int getParenthesesNumber(Char ch);

private:
    void updateStat(Char ch);

private:
    std::istream& istream;
    CodeStat stat;
};

}  // namespace AntLisp
