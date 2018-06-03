#include "math.h"

#include <antlisp/lib/cell/string/string.h>

#include <antlisp/lib/test/ut.h>

#include <iostream>


void test_sum_integer() {
    auto sum = AntLisp::Builtin::Sum();
    auto args = AntLisp::Arguments{};
    args.push_back(AntLisp::Cell::integer(192));
    args.push_back(AntLisp::Cell::integer(168));
    args.push_back(AntLisp::Cell::integer(1));
    args.push_back(AntLisp::Cell::integer(1));
    UT_ASSERT_EQUAL(
        sum.instantCall(
            std::move(args)
        ).as<AntLisp::Integer>(),
        AntLisp::Integer{192 + 168 + 1 + 1}
    );
}

void test_sum_integer_and_real() {
    auto sum = AntLisp::Builtin::Sum();
    auto args = AntLisp::Arguments{};
    args.push_back(AntLisp::Cell::integer(92));
    args.push_back(AntLisp::Cell::real(16.99));
    auto out = sum.instantCall(
        std::move(args)
    ).as<AntLisp::Float>();
    UT_ASSERT(out < 16 + 1.0 + 92);
    UT_ASSERT(out > 16 + 0.98 + 92);
}

void test_sum_symbol_and_string() {
    auto sum = AntLisp::Builtin::Sum();
    auto args = AntLisp::Arguments{};
    args.push_back(AntLisp::Cell::ext<AntLisp::StringType>(""));
    args.push_back(AntLisp::Cell::symbol('$'));
    args.push_back(AntLisp::Cell::ext<AntLisp::StringType>("in the"));
    args.push_back(AntLisp::Cell::symbol(' '));
    args.push_back(AntLisp::Cell::ext<AntLisp::StringType>("dark"));
    args.push_back(AntLisp::Cell::symbol('$'));
    auto out = sum.instantCall(
        std::move(args)
    );
    UT_ASSERT_EQUAL(
        out.as<AntLisp::StringType>(),
        AntLisp::StringType("$in the dark$")
    );
}

UT_LIST(
    RUN_TEST(test_sum_integer);
    RUN_TEST(test_sum_integer_and_real);
    //RUN_TEST(test_sum_symbol_and_string);
);
