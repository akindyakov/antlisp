#pragma once

#include <antlisp/lib/util/exception.h>


namespace AntLisp {

using Char = char;

struct CodeStat {
    // change name to level or depth
    int parentheses = 0;
    std::size_t lines = 1;
    std::size_t characters = 0;

    std::string toString() const {
        auto out = std::ostringstream{};
        out << "line " << lines << ", column " << characters;
        return out.str();
    }
};

class InCodeStream
{
public:
    explicit InCodeStream(
        std::istream& in
    );

    InCodeStream(const InCodeStream&) = delete;
    InCodeStream& operator=(const InCodeStream&) = delete;

    InCodeStream(InCodeStream&&) = default;
    InCodeStream& operator=(InCodeStream&&) = default;

    class Error
        : public Exception
    {
    };

    bool good() const;

    bool nextToken(
        std::string& token
    );
    std::string nextToken();

    bool tryOpen();
    bool tryClose();

    const CodeStat& getStat() const;

private:
    void skipSpaces();

    enum Parentheses
        : Char
    {
        CHAR_OPEN = '(',
        CHAR_CLOSE = ')',
    };

    bool next(Char& ch);
    void unget();

    struct Symbol {
        Char ch;
        bool escaped = false;
    };
    bool next(Symbol& sym);

    static int getParenthesesNumber(Char ch);

private:
    std::istream& istream;
    CodeStat stat;
};

}  // namespace AntLisp
