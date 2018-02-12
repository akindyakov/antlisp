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

class InCodeStream
{
public:
    explicit InCodeStream(
        std::istream& in
    )
        : istream(in)
    {
    }

    class Error
        : public Exception
    {
    };

    bool good() const {
        return (
            istream.good()
            && parenthesesCounter >= 0
        );
    }

    int pCount() const {
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
        token.clear();
        if (!this->good()) {
            throw Error() << "TODO: " << __FILE__ << __LINE__;
        }
        this->skipSpaces();
        auto ch = Char{};
        while (
            this->peek(ch)
            && std::isspace(ch) == false
        ) {
            if (getCounter(ch) != 0) {
                while (
                    this->peek(ch)
                    && getCounter(ch) != 0
                ) {
                    this->ignore();
                }
                break;
            }
            token.push_back(ch);
            this->ignore();
        }
        return !token.empty();
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

    static int getCounter(Char ch) {
        if (ch == CHAR_OPEN) {
            return 1;
        } else if (ch == CHAR_CLOSE) {
            return -1;
        }
        return 0;
    }

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

    bool isLocked() const {
        return level < codeStream.pCount();
    }

    bool good() const {
        return (
            level == codeStream.pCount()
            && codeStream.good()
        );
    }

    bool nextToken(
        std::string& token
    ) {
        return this->good() && codeStream.nextToken(token);
    }

    std::string nextToken() {
        auto token = std::string{};
        if (!this->nextToken(token)) {
            throw Error() << "Unexpected end of stream";
        }
        return token;
    }

    ParenthesesParser nextParser() {
        return ParenthesesParser(
            codeStream,
            codeStream.pCount()
        );
    }

private:
    explicit ParenthesesParser(
        InCodeStream& reader_
        , int level_
    )
        : codeStream(reader_)
        , level(level_)
    {
    }

private:
    InCodeStream& codeStream;
    int level = 0;
};

}  // namespace AntLisp
