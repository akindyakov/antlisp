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
        && parenthesesCounter >= 0
    );
}

int InCodeStream::pCount() const {
    return parenthesesCounter;
}

Char InCodeStream::peek() const {
    return static_cast<Char>(
        istream.peek()
    );
}

bool InCodeStream::peek(Char& ch) const {
    ch = istream.peek();
    return this->good();
}

bool InCodeStream::next(Char& ch) {
    if (this->good() && istream.get(ch).good()) {
        parenthesesCounter += getParenthesesNumber(ch);
        return true;
    }
    return false;
}

bool InCodeStream::ignore() {
    auto ch = Char{};
    return this->next(ch);
}

bool InCodeStream::nextToken(
    std::string& token
) {
    token.clear();
    if (not this->good()) {
        throw Error() << "TODO: " << __FILE__ << __LINE__;
    }
    this->skipSpaces();
    auto ch = Char{};
    while (
        this->peek(ch)
        && std::isspace(ch) == false
        && getParenthesesNumber(ch) == 0
    ) {
        token.push_back(ch);
        this->ignore();
    }
    this->skipSpaces();
    this->peek(ch);
    if (0 != InCodeStream::getParenthesesNumber(ch)) {
        this->ignore();
        this->skipSpaces();
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
    while (
        this->good()
        && std::isspace(
            istream.peek()
        )
    ) {
        istream.ignore();
    }
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
