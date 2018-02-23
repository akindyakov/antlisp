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

class ParenthesesParser
{
public:
    ParenthesesParser() = delete;
    ParenthesesParser(const ParenthesesParser&) = delete;
    ParenthesesParser(ParenthesesParser&&) = default;

    ParenthesesParser& operator=(const ParenthesesParser&) = delete;
    ParenthesesParser& operator=(ParenthesesParser&&) = default;

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

FunctionDefinitionPtr parseCode(
    std::istream& in
    , Namespace& global
);

}  // namespace AntLisp
