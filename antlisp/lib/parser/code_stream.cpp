#include "code_stream.h"


namespace AntLisp {

InCodeStream::InCodeStream(
    std::istream& in
)
    : istream(in)
{
}

bool InCodeStream::good() const {
    return (
        istream.good()
        && not istream.eof()
        && stat.parentheses >= 0
    );
}

bool InCodeStream::next(Char& ch) {
    if (this->good() && istream.get(ch).good()) {
        ++stat.characters;
        stat.lines += (ch == '\n') ? 1 : 0;
        return true;
    }
    return false;
}

void InCodeStream::unget() {
    istream.unget();
    --stat.characters;
}

bool InCodeStream::next(Symbol& symbol) {
    if (this->next(symbol.ch)) {
        symbol.escaped = (symbol.ch == '\\');
        if (symbol.escaped) {
            return this->next(symbol.ch);
        }
    } else {
        return false;
    }
    return true;
}

bool InCodeStream::nextToken(
    std::string& token
) {
    token.clear();
    if (not this->good()) {
        throw Error() << "TODO: " << __FILE__ << __LINE__;
    }
    this->skipSpaces();
    auto symbol = Symbol{};
    if (not this->next(symbol)) {
        return false;
    }
    if (0 != getParenthesesNumber(symbol.ch) and not symbol.escaped) {
        this->unget();
    } else if (symbol.ch == '"' and not symbol.escaped) {
        token.push_back(symbol.ch);
        while (this->next(symbol)) {
            token.push_back(symbol.ch);
            if (symbol.ch == '"' and not symbol.escaped) {
                break;
            }
        }
    } else {
        token.push_back(symbol.ch);
        while (this->next(symbol)) {
            if (not symbol.escaped) {
                if (std::isspace(symbol.ch)) {
                    break;
                }
                if (0 != getParenthesesNumber(symbol.ch)) {
                    this->unget();
                    break;
                }
            }
            token.push_back(symbol.ch);
        }
    }
    return !token.empty();
}

std::string InCodeStream::nextToken() {
    auto token = std::string{};
    if (!this->nextToken(token)) {
        throw Error() << "Unexpected end of stream";
    }
    return token;
}

void InCodeStream::skipSpaces() {
    auto ch = Char{};
    while (
        this->good()
        && std::isspace(
            this->istream.peek()
        )
    ) {
        this->next(ch);
    }
}

const CodeStat& InCodeStream::getStat() const {
    return stat;
}

bool InCodeStream::tryOpen() {
    this->skipSpaces();
    Char ch = istream.peek();
    if (getParenthesesNumber(ch) > 0) {
        ++stat.parentheses;
        this->next(ch);
    } else {
        return false;
    }
    return true;
}

bool InCodeStream::tryClose() {
    this->skipSpaces();
    Char ch = istream.peek();
    if (getParenthesesNumber(ch) < 0) {
        --stat.parentheses;
        this->next(ch);
    } else {
        return false;
    }
    return true;
}

int InCodeStream::getParenthesesNumber(Char ch) {
    if (ch == CHAR_OPEN) {
        return 1;
    } else if (ch == CHAR_CLOSE) {
        return -1;
    }
    return 0;
}

}  // namespace AntLisp
