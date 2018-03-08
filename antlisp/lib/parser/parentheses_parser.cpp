#include "parentheses_parser.h"

#include <antlisp/lib/util/string.h>

#include <iterator>


namespace AntLisp {

ParenthesesParser ParenthesesParser::fromCodeStream(
    InCodeStream& codeStream
) {
    return ParenthesesParser(
        codeStream,
        codeStream.pCount()
    );
}

bool ParenthesesParser::isLocked() const {
    return this->level != codeStream.pCount();
}

bool ParenthesesParser::isEnd() const {
    return (
        codeStream.pCount() < this->level
        || not codeStream.good()
    );
}

bool ParenthesesParser::good() const {
    return not (
        this->isEnd()
        || this->isLocked()
    );
}

bool ParenthesesParser::nextToken(
    std::string& token
) {
    if (this->good()) {
        if (codeStream.nextToken(token)) {
            return true;
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
    if (this->good()) {
        auto token = std::string{};
        if (this->nextToken(token)) {
            throw Error() << __FILE__ << ":" << __LINE__
                << " wtf, there is token " << Str::Quotes(token);
        }
        return not this->isEnd();
    }
    return false;
}

void ParenthesesParser::close() {
    if (this->good()) {
        codeStream.skipSpaces();
        auto ch = codeStream.peek();
        if (InCodeStream::getParenthesesNumber(ch) < 0) {
            // skip last closing ')'
            codeStream.ignore();
        } else {
            throw Error() << "Closing parenthes is expected here, got " << ch << "\n";
        }
    }
}

ParenthesesParser ParenthesesParser::nextParser() {
    if (not this->isLocked()) {
        throw Error() << "Parser should be locked to go deeper";
    }
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
