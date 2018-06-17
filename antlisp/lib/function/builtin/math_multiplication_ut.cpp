#include "math.h"

#include <antlisp/lib/cell_ext/string/string.h>

#include <antlisp/lib/test/ut.h>

#include <iostream>


void test_integer_multiplication() {
    auto m = AntLisp::Builtin::Multiplication();
    auto args = AntLisp::Arguments{};
    args.push_back(AntLisp::Cell::integer(12));
    args.push_back(AntLisp::Cell::integer(-6));
    UT_ASSERT_EQUAL(
        m.instantCall(
            std::move(args)
        ).as<AntLisp::Integer>(),
        AntLisp::Integer{12 * -6}
    );
}

void test_integer_and_real_multiplication() {
    auto m = AntLisp::Builtin::Multiplication();
    auto args = AntLisp::Arguments{};
    args.push_back(AntLisp::Cell::integer(15));
    args.push_back(AntLisp::Cell::real(5.8));
    auto out = m.instantCall(
        std::move(args)
    ).as<AntLisp::Float>();
    UT_ASSERT(out < 15 * 5.81);
    UT_ASSERT(out > 15 * 5.79);
}

void test_real_multiplication() {
    auto m = AntLisp::Builtin::Multiplication();
    auto args = AntLisp::Arguments{};
    args.push_back(AntLisp::Cell::real(5.45));
    args.push_back(AntLisp::Cell::real(6.1));
    auto out = m.instantCall(
        std::move(args)
    ).as<AntLisp::Float>();
    UT_ASSERT(out < 5.451 * 6.1);
    UT_ASSERT(out > 5.449 * 6.1);
}

void test_symbol_and_1_multiplication() {
    auto m = AntLisp::Builtin::Multiplication();
    auto args = AntLisp::Arguments{};
    args.push_back(AntLisp::Cell::symbol('s'));
    args.push_back(AntLisp::Cell::integer(1));
    UT_ASSERT_EQUAL(
        m.instantCall(
            std::move(args)
        ).as<AntLisp::StringType>(),
        AntLisp::StringType{"s"}
    );
}

void test_symbol_and_integer_multiplication() {
    auto m = AntLisp::Builtin::Multiplication();
    auto args = AntLisp::Arguments{};
    args.push_back(AntLisp::Cell::symbol('s'));
    args.push_back(AntLisp::Cell::integer(3));
    UT_ASSERT_EQUAL(
        m.instantCall(
            std::move(args)
        ).as<AntLisp::StringType>(),
        AntLisp::StringType{"sss"}
    );
}

void test_string_and_1_multiplication() {
    auto m = AntLisp::Builtin::Multiplication();
    auto args = AntLisp::Arguments{};
    args.push_back(AntLisp::Cell::ext<AntLisp::StringType>("welcomE"));
    args.push_back(AntLisp::Cell::integer(1));
    UT_ASSERT_EQUAL(
        m.instantCall(
            std::move(args)
        ).as<AntLisp::StringType>(),
        AntLisp::StringType("welcomE")
    );
}

void test_string_and_integer_multiplication() {
    auto m = AntLisp::Builtin::Multiplication();
    auto args = AntLisp::Arguments{};
    args.push_back(AntLisp::Cell::ext<AntLisp::StringType>("Basic"));
    args.push_back(AntLisp::Cell::integer(2));
    UT_ASSERT_EQUAL(
        m.instantCall(
            std::move(args)
        ).as<AntLisp::StringType>(),
        AntLisp::StringType("BasicBasic")
    );
}

UT_LIST(
    RUN_TEST(test_integer_multiplication);
    RUN_TEST(test_integer_and_real_multiplication);
    RUN_TEST(test_real_multiplication);
    //RUN_TEST(test_symbol_and_1_multiplication);
    //RUN_TEST(test_symbol_and_integer_multiplication);
    //RUN_TEST(test_string_and_1_multiplication);
    //RUN_TEST(test_string_and_integer_multiplication);
);
