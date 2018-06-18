#pragma once

#include "error.h"

#include <antlisp/lib/function/tape_machine.h>

#include <bitset>


namespace AntLisp {

enum class Keywords
    : std::size_t
{
    Defun,
    Lambda,
    Let,
    Set,
    Cond,
    Progn,
    Load,
    AllKeywords,
};

class ParserOptions {
public:
    /**
    * All keyword are switched on by default
    */
    explicit ParserOptions();

    template<
        Keywords keyword
    >
    ParserOptions& set();

    template<
        Keywords keyword
    >
    ParserOptions& unset();

    template<
        Keywords keyword
    >
    bool test() const;

private:
    std::bitset<static_cast<std::size_t>(Keywords::AllKeywords)> keywords_;
};

NativeFunction parseCode(
    std::istream& in
    , Namespace global
    , const ParserOptions& opts = ParserOptions()
);

}  // namespace AntLisp

#include "parser_impl.h"
