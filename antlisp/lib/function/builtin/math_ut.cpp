#include "math.h"

#include <antlisp/lib/test/ut.h>

#include <iostream>


void test_equality() {
    auto eq = AntLisp::Builtin::Equality();
    UT_ASSERT_EQUAL(
        eq.instantCall(
            AntLisp::Arguments{
                AntLisp::Cell::integer(48),
                AntLisp::Cell::integer(48),
            }
        ),
        AntLisp::Cell::t()
    );
    UT_ASSERT_EQUAL(
        eq.instantCall(
            AntLisp::Arguments{
                AntLisp::Cell::integer(155),
                AntLisp::Cell::integer(-155),
            }
        ),
        AntLisp::Cell::nil()
    );
    UT_ASSERT_EQUAL(
        eq.instantCall(
            AntLisp::Arguments{
                AntLisp::Cell::symbol('t'),
                AntLisp::Cell::symbol('t'),
            }
        ),
        AntLisp::Cell::t()
    );
    UT_ASSERT_EQUAL(
        eq.instantCall(
            AntLisp::Arguments{
                AntLisp::Cell::symbol('a'),
                AntLisp::Cell::symbol('A'),
            }
        ),
        AntLisp::Cell::nil()
    );
    UT_ASSERT_EQUAL(
        eq.instantCall(
            AntLisp::Arguments{
                AntLisp::Cell::string("keymaster"),
                AntLisp::Cell::string("keymaster"),
            }
        ),
        AntLisp::Cell::t()
    );
    UT_ASSERT_EQUAL(
        eq.instantCall(
            AntLisp::Arguments{
                AntLisp::Cell::string("key master"),
                AntLisp::Cell::string("keymaster"),
            }
        ),
        AntLisp::Cell::nil()
    );
    UT_ASSERT_EQUAL(
        eq.instantCall(
            AntLisp::Arguments{
                AntLisp::Cell::symbol('t'),
                AntLisp::Cell::symbol('t'),
                AntLisp::Cell::symbol('t'),
                AntLisp::Cell::symbol('t'),
                AntLisp::Cell::symbol('t'),
            }
        ),
        AntLisp::Cell::t()
    );
    UT_ASSERT_EQUAL(
        eq.instantCall(
            AntLisp::Arguments{
                AntLisp::Cell::symbol('u'),
                AntLisp::Cell::symbol('u'),
                AntLisp::Cell::symbol('*'),
                AntLisp::Cell::symbol('u'),
            }
        ),
        AntLisp::Cell::nil()
    );
}

void test_allMathBuiltinFunctions() {
    auto space = AntLisp::Namespace{};
    AntLisp::Builtin::allMathFunctions(space);
    const auto sumPtr = space.at("+").get<AntLisp::FunctionPtr>();
    UT_ASSERT_EQUAL(
        sumPtr->instantCall(
            AntLisp::Arguments{
                AntLisp::Cell::integer(17),
                AntLisp::Cell::integer(16),
            }
        ),
        AntLisp::Cell::integer(17 + 16)
    );
    const auto lessPtr = space.at("<").get<AntLisp::FunctionPtr>();
    UT_ASSERT_EQUAL(
        lessPtr->instantCall(
            AntLisp::Arguments{
                AntLisp::Cell::real(1.29),
                AntLisp::Cell::real(1.31),
            }
        ),
        AntLisp::Cell::t()
    );
    const auto eqPtr = space.at("=").get<AntLisp::FunctionPtr>();
    UT_ASSERT_EQUAL(
        eqPtr->instantCall(
            AntLisp::Arguments{
                AntLisp::Cell::real(1.29),
                AntLisp::Cell::real(1.29),
            }
        ),
        AntLisp::Cell::t()
    );
    const auto mPtr = space.at("*").get<AntLisp::FunctionPtr>();
    UT_ASSERT_EQUAL(
        mPtr->instantCall(
            AntLisp::Arguments{
                AntLisp::Cell::integer(6),
                AntLisp::Cell::integer(29),
            }
        ),
        AntLisp::Cell::integer(6 * 29)
    );
}

UT_LIST(
    RUN_TEST(test_equality);
    RUN_TEST(test_allMathBuiltinFunctions);
);
