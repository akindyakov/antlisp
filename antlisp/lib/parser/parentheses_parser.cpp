#include "parentheses_parser.h"

#include <antlisp/lib/util/string.h>

#include <iterator>


namespace AntLisp {

ParenthesesParser ParenthesesParser::openFromCodeStream(
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

ParenthesesParser ParenthesesParser::fromCodeStream(
    InCodeStream& codeStream
) {
    return ParenthesesParser(
        codeStream,
        codeStream.pCount()
    );
}

bool ParenthesesParser::isLocked() const {
    return this->level < codeStream.pCount();
}

bool ParenthesesParser::isEnd() const {
    return (
        codeStream.pCount() < this->level
        || not codeStream.good()
    );
}

bool ParenthesesParser::nextToken(
    std::string& token
) {
    //if (not this->isLocked() && this->good()) {
    if (not this->isEnd()) {
        if (codeStream.nextToken(token)) {
            return true;
        }
        auto ch = codeStream.peek();
        if (0 != InCodeStream::getParenthesesNumber(ch)) {
            // FIXME: move to to code stream class
            codeStream.ignore();
        }
        return false;
    }
    return false;
}

std::string ParenthesesParser::nextToken() {
    auto token = std::string{};
    if (!this->nextToken(token)) {
        throw Error() << "Unexpected end of stream";
    }
    return token;
}

bool ParenthesesParser::check() {
    if (not this->isEnd()) {
        auto token = std::string{};
        if (this->nextToken(token)) {
            throw Error() << __FILE__ << ":" << __LINE__
                << " wtf, there is token " << Str::Quotes(token);
        }
        return not this->isEnd();
    }
    return false;
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
