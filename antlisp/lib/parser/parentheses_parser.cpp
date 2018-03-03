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
    return level < codeStream.pCount();
}

bool ParenthesesParser::good() const {
    //**/ std::cerr << level << " == " << codeStream.pCount() << '\n';
    return (
        level <= codeStream.pCount()
        && codeStream.good()
    );
}

bool ParenthesesParser::nextToken(
    std::string& token
) {
    if (this->good()) {
        if (codeStream.nextToken(token)) {
            return true;
        }
        auto ch = codeStream.peek();
        if (0 != InCodeStream::getParenthesesNumber(ch)) {
            //**/ std::cerr << "skip the last parenthesis to close parser " << ch << "\n";
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
    //**/ std::cerr << "new token " << Str::Quotes(token) << '\n';
    return token;
}

bool ParenthesesParser::check() {
    if (this->good()) {
        auto token = std::string{};
        if (this->nextToken(token)) {
            throw Error() << __FILE__ << ":" << __LINE__
                << " wtf, there is token " << Str::Quotes(token);
        }
        return this->good();
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
