#include "math.h"

#include <antlisp/lib/test/ut.h>

#include <iostream>


void test_div_integer() {
    auto div = AntLisp::Builtin::Division();
    UT_ASSERT_EQUAL(
        div.instantCall(
            AntLisp::Arguments{
                AntLisp::Cell::integer(128),
                AntLisp::Cell::integer(4),
                AntLisp::Cell::integer(2),
                AntLisp::Cell::integer(-1),
                AntLisp::Cell::integer(-2),
            }
        ),
        AntLisp::Cell::integer((((128 / 4) / 2) / -1) / -2)
    );
}

void test_div_integer_and_real() {
    auto sum = AntLisp::Builtin::Division();
    auto out = sum.instantCall(
        AntLisp::Arguments{
            AntLisp::Cell::integer(13),
            AntLisp::Cell::integer(2),
            AntLisp::Cell::integer(-3),
            AntLisp::Cell::real(-4.0),
        }
    ).get<AntLisp::Float>();
    UT_ASSERT(out < 0.5 + 0.001);
    UT_ASSERT(out > 0.5 - 0.001);
}

UT_LIST(
    RUN_TEST(test_div_integer);
    RUN_TEST(test_div_integer_and_real);
);
