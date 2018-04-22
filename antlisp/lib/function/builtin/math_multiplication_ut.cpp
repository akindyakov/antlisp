#include "math.h"

#include <antlisp/lib/cell/ext/string/string.h>

#include <antlisp/lib/test/ut.h>

#include <iostream>


void test_integer_multiplication() {
    auto m = AntLisp::Builtin::Multiplication();
    UT_ASSERT_EQUAL(
        m.instantCall(
            AntLisp::Arguments{
                AntLisp::Cell::integer(12),
                AntLisp::Cell::integer(-6),
            }
        ),
        AntLisp::Cell::integer(12 * -6)
    );
}

void test_integer_and_real_multiplication() {
    auto m = AntLisp::Builtin::Multiplication();
    auto out = m.instantCall(
        AntLisp::Arguments{
            AntLisp::Cell::integer(15),
            AntLisp::Cell::real(5.8),
        }
    ).get<AntLisp::Float>();
    UT_ASSERT(out < 15 * 5.81);
    UT_ASSERT(out > 15 * 5.79);
}

void test_real_multiplication() {
    auto m = AntLisp::Builtin::Multiplication();
    auto out = m.instantCall(
        AntLisp::Arguments{
            AntLisp::Cell::real(5.45),
            AntLisp::Cell::real(6.1),
        }
    ).get<AntLisp::Float>();
    UT_ASSERT(out < 5.451 * 6.1);
    UT_ASSERT(out > 5.449 * 6.1);
}

void test_symbol_and_1_multiplication() {
    auto m = AntLisp::Builtin::Multiplication();
    UT_ASSERT_EQUAL(
        m.instantCall(
            AntLisp::Arguments{
                AntLisp::Cell::symbol('s'),
                AntLisp::Cell::integer(1),
            }
        ),
        AntLisp::Cell::ext<AntLisp::StringType>("s")
    );
}

void test_symbol_and_integer_multiplication() {
    auto m = AntLisp::Builtin::Multiplication();
    UT_ASSERT_EQUAL(
        m.instantCall(
            AntLisp::Arguments{
                AntLisp::Cell::symbol('s'),
                AntLisp::Cell::integer(3),
            }
        ),
        AntLisp::Cell::ext<AntLisp::StringType>("sss")
    );
}

void test_string_and_1_multiplication() {
    auto m = AntLisp::Builtin::Multiplication();
    UT_ASSERT_EQUAL(
        m.instantCall(
            AntLisp::Arguments{
                AntLisp::Cell::ext<AntLisp::StringType>("welcomE"),
                AntLisp::Cell::integer(1),
            }
        ),
        AntLisp::Cell::ext<AntLisp::StringType>("welcomE")
    );
}

void test_string_and_integer_multiplication() {
    auto m = AntLisp::Builtin::Multiplication();
    UT_ASSERT_EQUAL(
        m.instantCall(
            AntLisp::Arguments{
                AntLisp::Cell::ext<AntLisp::StringType>("Basic"),
                AntLisp::Cell::integer(2),
            }
        ),
        AntLisp::Cell::ext<AntLisp::StringType>("BasicBasic")
    );
}

UT_LIST(
    RUN_TEST(test_integer_multiplication);
    RUN_TEST(test_integer_and_real_multiplication);
    RUN_TEST(test_real_multiplication);
    //RUN_TEST(test_symbol_and_1_multiplication);
    //RUN_TEST(test_symbol_and_integer_multiplication);
    RUN_TEST(test_string_and_1_multiplication);
    RUN_TEST(test_string_and_integer_multiplication);
);
