#include "parentheses_parser.h"

#include <antlisp/lib/util/string.h>

#include <iterator>


namespace AntLisp {

ParenthesesParser::~ParenthesesParser() {
    if (level_ >= 0) {
        std::cerr
            << "[debug] " << getStat().toString()
            << " Warning: deleting unclosed ParenthesesParser"
            << " on depth " << level_ << '\n';
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
    return level_ != this->getStat().parentheses;
}

bool ParenthesesParser::isEnd() const {
    return (
        level_ < 0
        || getStat().parentheses < level_
        || not codeStream_.good()
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
        if (codeStream_.nextToken(token)) {
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
    if (level_ > 0) {
        if (not codeStream_.tryClose()) {
            throw Error()
                << this->getStat().toString()
                << " Wrong attempt to close unfinished expression";
        }
    }
    level_ = -1;
}

Optional<ParenthesesParser> ParenthesesParser::nextParser() {
    if (codeStream_.tryOpen()) {
        return ParenthesesParser(
            codeStream_,
            codeStream_.getStat().parentheses
        );
    }
    return Optional<ParenthesesParser>{};
}

ParenthesesParser::ParenthesesParser(
    InCodeStream& reader_
    , int level
)
    : codeStream_(reader_)
    , level_(level)
{
}

}  // namespace AntLisp
