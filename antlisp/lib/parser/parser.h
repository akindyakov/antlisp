#pragma once

#include <antlisp/lib/util/exception.h>

#include <antlisp/lib/function/tape_machine.h>

#include "error.h"

#include <bitset>


namespace AntLisp {

class ParserOptions {
public:
    /**
    * All keyword are switched on by default
    */
    // TODO(akindyakov): test ParserOptions please
    explicit ParserOptions();

    enum EKeywords
        : std::size_t
    {
        Defun,
        Lambda,
        Let,
        Set,
        Cond,
        Progn,
        UpLimit,
    };

    ParserOptions& setDefun();
    ParserOptions& setLambda();
    ParserOptions& setLet();
    ParserOptions& setSet();
    ParserOptions& setCond();
    ParserOptions& setProgn();
    ParserOptions& setAllKeywords();

    ParserOptions& unsetDefun();
    ParserOptions& unsetLambda();
    ParserOptions& unsetLet();
    ParserOptions& unsetSet();
    ParserOptions& unsetCond();
    ParserOptions& unsetProgn();
    ParserOptions& unsetAllKeywords();

    bool testDefun() const;
    bool testLambda() const;
    bool testLet() const;
    bool testSet() const;
    bool testCond() const;
    bool testProgn() const;

    bool testKeyword(
        EKeywords keyword
    ) const;

private:
    std::bitset<static_cast<std::size_t>(EKeywords::UpLimit)> keywords_;
};

NativeFunction parseCode(
    std::istream& in
    , Namespace global
    , const ParserOptions& opts = ParserOptions()
);

}  // namespace AntLisp
