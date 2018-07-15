#include "math.h"

#include <antlisp/lib/cell_ext/string/string.h>

#include <antlisp/lib/test/ut.h>

#include <iostream>


void test_equality_int() {
    auto eq = AntLisp::Builtin::Equality();
    auto args = AntLisp::Arguments{};
    args.push_back(AntLisp::Cell::integer(48));
    args.push_back(AntLisp::Cell::integer(48));
    UT_ASSERT(
        eq.instantCall(
            std::move(args)
        ).is<AntLisp::Integer>()
    );
}

void test_equality_int_neg() {
    auto eq = AntLisp::Builtin::Equality();
    auto args = AntLisp::Arguments{};
    args.push_back(AntLisp::Cell::integer(155));
    args.push_back(AntLisp::Cell::integer(-155));
    auto ans = eq.instantCall(
        std::move(args)
    );
    UT_ASSERT(
        ans.is<AntLisp::Nil>()
    );
}

void test_equality_symbol() {
    auto eq = AntLisp::Builtin::Equality();
    auto args = AntLisp::Arguments{};
    args.push_back(AntLisp::Cell::symbol('t'));
    args.push_back(AntLisp::Cell::symbol('t'));
    UT_ASSERT(
        eq.instantCall(
            std::move(args)
        ).is<AntLisp::Integer>()
    );
}

void test_equality_symbol_nil() {
    auto eq = AntLisp::Builtin::Equality();
    auto args = AntLisp::Arguments{};
    args.push_back(AntLisp::Cell::symbol('a'));
    args.push_back(AntLisp::Cell::symbol('A'));
    UT_ASSERT(
        eq.instantCall(
            std::move(args)
        ).is<AntLisp::Nil>()
    );
}

void test_equality_string() {
    auto eq = AntLisp::Builtin::Equality();
    auto args = AntLisp::Arguments{};
    args.push_back(AntLisp::Cell::ext<AntLisp::StringType>("keymaster"));
    args.push_back(AntLisp::Cell::ext<AntLisp::StringType>("keymaster"));
    UT_ASSERT(
        eq.instantCall(
            std::move(args)
        ).is<AntLisp::Integer>()
    );
}

void test_equality_string_nil() {
    auto eq = AntLisp::Builtin::Equality();
    auto args = AntLisp::Arguments{};
    args.push_back(AntLisp::Cell::ext<AntLisp::StringType>("key master"));
    args.push_back(AntLisp::Cell::ext<AntLisp::StringType>("keymaster"));
    UT_ASSERT(
        eq.instantCall(
            std::move(args)
        ).is<AntLisp::Nil>()
    );
}

void test_equality_multi_symbol() {
    auto eq = AntLisp::Builtin::Equality();
    auto args = AntLisp::Arguments{};
    args.push_back(AntLisp::Cell::symbol('t'));
    args.push_back(AntLisp::Cell::symbol('t'));
    args.push_back(AntLisp::Cell::symbol('t'));
    args.push_back(AntLisp::Cell::symbol('t'));
    args.push_back(AntLisp::Cell::symbol('t'));
    UT_ASSERT(
        eq.instantCall(
            std::move(args)
        ).is<AntLisp::Integer>()
    );
}

void test_equality_multi_symbol_nil() {
    auto eq = AntLisp::Builtin::Equality();
    auto args = AntLisp::Arguments{};
    args.push_back(AntLisp::Cell::symbol('u'));
    args.push_back(AntLisp::Cell::symbol('u'));
    args.push_back(AntLisp::Cell::symbol('*'));
    args.push_back(AntLisp::Cell::symbol('u'));
    UT_ASSERT(
        eq.instantCall(
            std::move(args)
        ).is<AntLisp::Nil>()
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
