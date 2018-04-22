#include "math.h"

#include <antlisp/lib/cell/ext/string/string.h>

#include <antlisp/lib/test/ut.h>

#include <iostream>


void test_equality_int() {
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
}

void test_equality_int_neg() {
    auto eq = AntLisp::Builtin::Equality();
    UT_ASSERT_EQUAL(
        eq.instantCall(
            AntLisp::Arguments{
                AntLisp::Cell::integer(155),
                AntLisp::Cell::integer(-155),
            }
        ),
        AntLisp::Cell::nil()
    );
}

void test_equality_symbol() {
    auto eq = AntLisp::Builtin::Equality();
    UT_ASSERT_EQUAL(
        eq.instantCall(
            AntLisp::Arguments{
                AntLisp::Cell::symbol('t'),
                AntLisp::Cell::symbol('t'),
            }
        ),
        AntLisp::Cell::t()
    );
}

void test_equality_symbol_nil() {
    auto eq = AntLisp::Builtin::Equality();
    UT_ASSERT_EQUAL(
        eq.instantCall(
            AntLisp::Arguments{
                AntLisp::Cell::symbol('a'),
                AntLisp::Cell::symbol('A'),
            }
        ),
        AntLisp::Cell::nil()
    );
}

void test_equality_string() {
    auto eq = AntLisp::Builtin::Equality();
    UT_ASSERT_EQUAL(
        eq.instantCall(
            AntLisp::Arguments{
                AntLisp::Cell::ext<AntLisp::StringType>("keymaster"),
                AntLisp::Cell::ext<AntLisp::StringType>("keymaster"),
            }
        ),
        AntLisp::Cell::t()
    );
}

void test_equality_string_nil() {
    auto eq = AntLisp::Builtin::Equality();
    UT_ASSERT_EQUAL(
        eq.instantCall(
            AntLisp::Arguments{
                AntLisp::Cell::ext<AntLisp::StringType>("key master"),
                AntLisp::Cell::ext<AntLisp::StringType>("keymaster"),
            }
        ),
        AntLisp::Cell::nil()
    );
}

void test_equality_multi_symbol() {
    auto eq = AntLisp::Builtin::Equality();
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
}

void test_equality_multi_symbol_nil() {
    auto eq = AntLisp::Builtin::Equality();
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

UT_LIST(
    RUN_TEST(test_equality_int);
    RUN_TEST(test_equality_int_neg);
    RUN_TEST(test_equality_symbol);
    RUN_TEST(test_equality_symbol_nil);
    RUN_TEST(test_equality_string);
    RUN_TEST(test_equality_string_nil);
    RUN_TEST(test_equality_multi_symbol);
    RUN_TEST(test_equality_multi_symbol_nil);
);
