#include "math.h"

#include <antlisp/lib/test/ut.h>

#include <iostream>


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
    const auto dPtr = space.at("/").get<AntLisp::FunctionPtr>();
    UT_ASSERT_EQUAL(
        dPtr->instantCall(
            AntLisp::Arguments{
                AntLisp::Cell::integer(143),
                AntLisp::Cell::integer(11),
            }
        ),
        AntLisp::Cell::integer(13)
    );
}

UT_LIST(
    RUN_TEST(test_allMathBuiltinFunctions);
);
