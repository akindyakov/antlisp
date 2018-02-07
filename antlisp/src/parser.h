#pragma once

#include "exception.h"


namespace AntLisp {

/**
* function call
* function definition
* let
* cond
* lambda
*/

using Char = char;

void SkipSpaces(
    std::istream& istream
) {
    while (
        istream.good()
        && std::isspace(
            istream.peek()
        )
    ) {
        istream.ignore();
    }
}

class ParenthesesExprReader
{
public:
    class Error
        : public Exception
    {
    };

    bool good() const {
        return (
            istream.good()
            && parenthesesCounter > 0
        );
    }

    int count() const {
        return parenthesesCounter;
    }

    Char peek() const {
        return static_cast<Char>(
            istream.peek()
        );
    }

    bool peek(Char& ch) const {
        ch = istream.peek();
        return this->good();
    }

    bool next(Char& ch) {
        if (this->good() && istream.get(ch).good()) {
            parenthesesCounter += getCounter(ch);
            return true;
        }
        return false;
    }

    bool ignore() {
        auto ch = Char{};
        return this->next(ch);
    }

    bool nextToken(
        std::string& token
    ) {
        skipSpaces();
        auto ch = Char{};
        while (
            this->peek(ch)
            && getCounter(ch) == 0
            && std::isspace(istream.peek()) == false
        ) {
            token.push_back(ch);
            istream.ignore();
        }
        return this->good() && !token.empty();
    }

    std::string nextToken() {
        auto token = std::string{};
        if (!this->nextToken(token)) {
            throw Error() << "Unexpected end of stream";
        }
        return token;
    }

    void skipSpaces() {
        while (
            this->good()
            && std::isspace(
                istream.peek()
            )
        ) {
            istream.ignore();
        }
    }

    enum Parentheses
        : Char
    {
        CHAR_OPEN = '(',
        CHAR_CLOSE = ')',
    };

    static ParenthesesExprReader createFromStream(
        std::istream& in
    ) {
        SkipSpaces(in);
        auto ch = in.get();
        if (ch != CHAR_OPEN) {
            throw Error();
        }
        return ParenthesesExprReader(in);
    }

private:
    explicit ParenthesesExprReader(
        std::istream& in
    )
        : istream(in)
    {
    }

    static inline int getCounter(Char ch) {
        if (ch == CHAR_OPEN) {
            return 1;
        } else if (ch == CHAR_CLOSE) {
            return -1;
        }
        return 0;
    }

private:
    std::istream& istream;
    int parenthesesCounter = 1;
};

}  // namespace AntLisp
