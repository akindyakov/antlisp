#include "math.h"

#include <antlisp/lib/test/ut.h>

#include <iostream>


void test_div_integer() {
    auto div = AntLisp::Builtin::Division();
    auto args = AntLisp::Arguments{};
    args.push_back(AntLisp::Cell::integer(128));
    args.push_back(AntLisp::Cell::integer(4));
    args.push_back(AntLisp::Cell::integer(2));
    args.push_back(AntLisp::Cell::integer(-1));
    args.push_back(AntLisp::Cell::integer(-2));
    UT_ASSERT_EQUAL(
        div.instantCall(
            std::move(args)
        ).as<AntLisp::Integer>(),
        AntLisp::Integer{(((128 / 4) / 2) / -1) / -2}
    );
}

void test_div_integer_and_real() {
    auto sum = AntLisp::Builtin::Division();
    auto args = AntLisp::Arguments{};
    args.push_back(AntLisp::Cell::integer(13));
    args.push_back(AntLisp::Cell::integer(2));
    args.push_back(AntLisp::Cell::integer(-3));
    args.push_back(AntLisp::Cell::real(-4.0));
    auto out = sum.instantCall(
        std::move(args)
    ).as<AntLisp::Float>();
    UT_ASSERT(out < 0.5 + 0.001);
    UT_ASSERT(out > 0.5 - 0.001);
}

UT_LIST(
    RUN_TEST(test_div_integer);
    RUN_TEST(test_div_integer_and_real);
);
