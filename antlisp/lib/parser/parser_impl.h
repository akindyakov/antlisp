#pragma once

namespace AntLisp {

template<
    Keywords keyword
>
ParserOptions& ParserOptions::set() {
    if (keyword == Keywords::AllKeywords) {
        keywords_.set();
    } else {
        keywords_.set(
            static_cast<std::size_t>(keyword)
        );
    }
    return *this;
}

template<
    Keywords keyword
>
ParserOptions& ParserOptions::unset() {
    if (keyword == Keywords::AllKeywords) {
        keywords_.reset();
    } else {
        keywords_.reset(
            static_cast<std::size_t>(keyword)
        );
    }
    return *this;
}

template<
    Keywords keyword
>
bool ParserOptions::test() const {
    if (keyword == Keywords::AllKeywords) {
        return keywords_.all();
    }
    return keywords_.test(
        static_cast<std::size_t>(keyword)
    );
}

}  // namespace AntLisp
