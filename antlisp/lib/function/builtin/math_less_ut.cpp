#include "math.h"

#include <antlisp/lib/cell/ext/string/string.h>

#include <antlisp/lib/test/ut.h>

#include <iostream>


void test_less_int_nil() {
    auto less = AntLisp::Builtin::Less();
    UT_ASSERT_EQUAL(
        less.instantCall(
            AntLisp::Arguments{
                AntLisp::Cell::integer(86),
                AntLisp::Cell::integer(64),
            }
        ),
        AntLisp::Cell::nil()
    );
}

void test_less_int_true() {
    auto less = AntLisp::Builtin::Less();
    UT_ASSERT_EQUAL(
        less.instantCall(
            AntLisp::Arguments{
                AntLisp::Cell::integer(32),
                AntLisp::Cell::integer(64),
            }
        ),
        AntLisp::Cell::t()
    );
}

void test_less_real_nil() {
    auto less = AntLisp::Builtin::Less();
    UT_ASSERT_EQUAL(
        less.instantCall(
            AntLisp::Arguments{
                AntLisp::Cell::real(192.168),
                AntLisp::Cell::real(1.155),
            }
        ),
        AntLisp::Cell::nil()
    );
}

void test_less_real_true() {
    auto less = AntLisp::Builtin::Less();
    UT_ASSERT_EQUAL(
        less.instantCall(
            AntLisp::Arguments{
                AntLisp::Cell::real(2.16),
                AntLisp::Cell::real(10.7),
            }
        ),
        AntLisp::Cell::t()
    );
}

void test_less_int_and_real_true() {
    auto less = AntLisp::Builtin::Less();
    UT_ASSERT_EQUAL(
        less.instantCall(
            AntLisp::Arguments{
                AntLisp::Cell::integer(2),
                AntLisp::Cell::real(10.7),
            }
        ),
        AntLisp::Cell::t()
    );
}

void test_less_neg_real_and_neg_int() {
    auto less = AntLisp::Builtin::Less();
    UT_ASSERT_EQUAL(
        less.instantCall(
            AntLisp::Arguments{
                AntLisp::Cell::real(-2.66),
                AntLisp::Cell::integer(-2),
            }
        ),
        AntLisp::Cell::t()
    );
}

void test_less_str_true() {
    auto less = AntLisp::Builtin::Less();
    UT_ASSERT_EQUAL(
        less.instantCall(
            AntLisp::Arguments{
                AntLisp::Cell::ext<AntLisp::StringType>("aa"),
                AntLisp::Cell::ext<AntLisp::StringType>("aaa"),
            }
        ),
        AntLisp::Cell::t()
    );
}

void test_less_str_nil() {
    auto less = AntLisp::Builtin::Less();
    UT_ASSERT_EQUAL(
        less.instantCall(
            AntLisp::Arguments{
                AntLisp::Cell::ext<AntLisp::StringType>("abc"),
                AntLisp::Cell::ext<AntLisp::StringType>("abc"),
            }
        ),
        AntLisp::Cell::nil()
    );
}

void test_less_symbol_true() {
    auto less = AntLisp::Builtin::Less();
    UT_ASSERT_EQUAL(
        less.instantCall(
            AntLisp::Arguments{
                AntLisp::Cell::symbol('a'),
                AntLisp::Cell::symbol('b'),
            }
        ),
        AntLisp::Cell::t()
    );
}

void test_less_symbol_nil() {
    auto less = AntLisp::Builtin::Less();
    UT_ASSERT_EQUAL(
        less.instantCall(
            AntLisp::Arguments{
                AntLisp::Cell::symbol('c'),
                AntLisp::Cell::symbol('b'),
            }
        ),
        AntLisp::Cell::nil()
    );
}

void test_less_multiple_symbol_true() {
    auto less = AntLisp::Builtin::Less();
    UT_ASSERT_EQUAL(
        less.instantCall(
            AntLisp::Arguments{
                AntLisp::Cell::symbol('a'),
                AntLisp::Cell::symbol('b'),
                AntLisp::Cell::symbol('c'),
                AntLisp::Cell::symbol('d'),
                AntLisp::Cell::symbol('e'),
                AntLisp::Cell::symbol('y'),
                AntLisp::Cell::symbol('z'),
            }
        ),
        AntLisp::Cell::t()
    );
}

void test_less_multiple_int_nil() {
    auto less = AntLisp::Builtin::Less();
    UT_ASSERT_EQUAL(
        less.instantCall(
            AntLisp::Arguments{
                AntLisp::Cell::integer(10),
                AntLisp::Cell::integer(20),
                AntLisp::Cell::integer(30),
                AntLisp::Cell::integer(30),
                AntLisp::Cell::integer(40),
            }
        ),
        AntLisp::Cell::nil()
    );
}

void test_less_multiple_symbol_nil() {
    auto less = AntLisp::Builtin::Less();
    UT_ASSERT_EQUAL(
        less.instantCall(
            AntLisp::Arguments{
                AntLisp::Cell::symbol('1'),
                AntLisp::Cell::symbol('2'),
                AntLisp::Cell::symbol('3'),
                AntLisp::Cell::symbol('3'),
                AntLisp::Cell::symbol('4'),
            }
        ),
        AntLisp::Cell::nil()
    );
}

UT_LIST(
    RUN_TEST(test_less_int_nil);
    RUN_TEST(test_less_int_true);
    RUN_TEST(test_less_real_nil);
    RUN_TEST(test_less_real_true);
    RUN_TEST(test_less_int_and_real_true);
    RUN_TEST(test_less_neg_real_and_neg_int);
    RUN_TEST(test_less_str_true);
    RUN_TEST(test_less_str_nil);
    RUN_TEST(test_less_symbol_true);
    RUN_TEST(test_less_symbol_nil);
    RUN_TEST(test_less_multiple_symbol_true);
    RUN_TEST(test_less_multiple_int_nil);
    RUN_TEST(test_less_multiple_symbol_nil);
);
