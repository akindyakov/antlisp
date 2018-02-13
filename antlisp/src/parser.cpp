#include "parser.h"


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
    if (!this->good()) {
        throw Error() << "TODO: " << __FILE__ << __LINE__;
    }
    this->skipSpaces();
    auto ch = Char{};
    while (
        this->peek(ch)
        && std::isspace(ch) == false
    ) {
        if (getParenthesesNumber(ch) != 0) {
            while (
                this->peek(ch)
                && getParenthesesNumber(ch) != 0
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

ParenthesesParser ParenthesesParser::fromCodeStream(
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

bool ParenthesesParser::isLocked() const {
    return level < codeStream.pCount();
}

bool ParenthesesParser::good() const {
    return (
        level == codeStream.pCount()
        && codeStream.good()
    );
}

bool ParenthesesParser::nextToken(
    std::string& token
) {
    return this->good() && codeStream.nextToken(token);
}

std::string ParenthesesParser::nextToken() {
    auto token = std::string{};
    if (!this->nextToken(token)) {
        throw Error() << "Unexpected end of stream";
    }
    return token;
}

ParenthesesParser ParenthesesParser::nextParser() {
    return ParenthesesParser(
        codeStream,
        codeStream.pCount()
    );
}

ParenthesesParser::ParenthesesParser(
    InCodeStream& reader_
    , int level_
)
    : codeStream(reader_)
    , level(level_)
{
}

}  // namespace AntLisp
