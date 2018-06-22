#include "parentheses_parser.h"

#include <antlisp/lib/util/string.h>

#include <iterator>


namespace AntLisp {

ParenthesesParser::~ParenthesesParser() {
    if (this->level >= 0) {
        std::cerr
            << "[debug] " << this->getStat().toString()
            << " Warning: deleting unclosed ParenthesesParser"
            << " on depth " << this->level << '\n';
    }
}

ParenthesesParser ParenthesesParser::fromCodeStream(
    InCodeStream& codeStream
) {
    return ParenthesesParser(
        codeStream,
        codeStream.getStat().parentheses
    );
}

bool ParenthesesParser::isLocked() const {
    return this->level != this->getStat().parentheses;
}

bool ParenthesesParser::isEnd() const {
    return (
        this->level < 0
        || this->getStat().parentheses < this->level
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
        throw Error() << "Unexpected end of stream " << this->getStat().toString();
    }
    return token;
}

void ParenthesesParser::close() {
    if (this->level > 0) {
        if (not codeStream.tryClose()) {
            throw Error()
                << this->getStat().toString()
                << " Wrong attempt to close unfinished expression";
        }
    }
    this->level = -1;
}

Optional<ParenthesesParser> ParenthesesParser::nextParser() {
    if (codeStream.tryOpen()) {
        return ParenthesesParser(
            codeStream,
            codeStream.getStat().parentheses
        );
    }
    return Optional<ParenthesesParser>{};
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