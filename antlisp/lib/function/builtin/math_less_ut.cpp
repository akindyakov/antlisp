#include "math.h"

#include <antlisp/lib/cell/string/string.h>

#include <antlisp/lib/test/ut.h>

#include <iostream>


void test_less_int_nil() {
    auto less = AntLisp::Builtin::Less();
    auto args = AntLisp::Arguments{};
    args.push_back(AntLisp::Cell::integer(86));
    args.push_back(AntLisp::Cell::integer(64));
    UT_ASSERT_EQUAL(
        less.instantCall(
            std::move(args)
        ).as<AntLisp::Nil>(),
        AntLisp::Nil{}
    );
}

void test_less_int_true() {
    auto less = AntLisp::Builtin::Less();
    auto args = AntLisp::Arguments{};
    args.push_back(AntLisp::Cell::integer(32));
    args.push_back(AntLisp::Cell::integer(64));
    UT_ASSERT_EQUAL(
        less.instantCall(
            std::move(args)
        ).as<AntLisp::Integer>(),
        AntLisp::Cell::t().as<AntLisp::Integer>()
    );
}

void test_less_real_nil() {
    auto less = AntLisp::Builtin::Less();
    auto args = AntLisp::Arguments{};
    args.push_back(AntLisp::Cell::real(192.168));
    args.push_back(AntLisp::Cell::real(1.155));
    UT_ASSERT_EQUAL(
        less.instantCall(
            std::move(args)
        ).as<AntLisp::Nil>(),
        AntLisp::Nil{}
    );
}

void test_less_real_true() {
    auto less = AntLisp::Builtin::Less();
    auto args = AntLisp::Arguments{};
    args.push_back(AntLisp::Cell::real(2.16));
    args.push_back(AntLisp::Cell::real(10.7));
    UT_ASSERT_EQUAL(
        less.instantCall(
            std::move(args)
        ).as<AntLisp::Integer>(),
        AntLisp::Cell::t().as<AntLisp::Integer>()
    );
}

void test_less_int_and_real_true() {
    auto less = AntLisp::Builtin::Less();
    auto args = AntLisp::Arguments{};
    args.push_back(AntLisp::Cell::integer(2));
    args.push_back(AntLisp::Cell::real(10.7));
    UT_ASSERT_EQUAL(
        less.instantCall(
            std::move(args)
        ).as<AntLisp::Integer>(),
        AntLisp::Cell::t().as<AntLisp::Integer>()
    );
}

void test_less_neg_real_and_neg_int() {
    auto less = AntLisp::Builtin::Less();
    auto args = AntLisp::Arguments{};
    args.push_back(AntLisp::Cell::real(-2.66));
    args.push_back(AntLisp::Cell::integer(-2));
    UT_ASSERT_EQUAL(
        less.instantCall(
            std::move(args)
        ).as<AntLisp::Integer>(),
        AntLisp::Cell::t().as<AntLisp::Integer>()
    );
}

void test_less_str_true() {
    auto less = AntLisp::Builtin::Less();
    auto args = AntLisp::Arguments{};
    args.push_back(AntLisp::Cell::ext<AntLisp::StringType>("aa"));
    args.push_back(AntLisp::Cell::ext<AntLisp::StringType>("aaa"));
    auto ans = less.instantCall(
        std::move(args)
    );
    UT_ASSERT(
        not ans.is<AntLisp::Nil>()
    );
}

void test_less_str_nil() {
    auto less = AntLisp::Builtin::Less();
    auto args = AntLisp::Arguments{};
    args.push_back(AntLisp::Cell::ext<AntLisp::StringType>("abc"));
    args.push_back(AntLisp::Cell::ext<AntLisp::StringType>("abc"));
    UT_ASSERT_EQUAL(
        less.instantCall(
            std::move(args)
        ).as<AntLisp::Nil>(),
        AntLisp::Nil{}
    );
}

void test_less_symbol_true() {
    auto less = AntLisp::Builtin::Less();
    auto args = AntLisp::Arguments{};
    args.push_back(AntLisp::Cell::symbol('a'));
    args.push_back(AntLisp::Cell::symbol('b'));
    auto ans = less.instantCall(
        std::move(args)
    );
    UT_ASSERT(
        not ans.is<AntLisp::Nil>()
    );
}

void test_less_symbol_nil() {
    auto less = AntLisp::Builtin::Less();
    auto args = AntLisp::Arguments{};
    args.push_back(AntLisp::Cell::symbol('c'));
    args.push_back(AntLisp::Cell::symbol('b'));
    UT_ASSERT_EQUAL(
        less.instantCall(
            std::move(args)
        ).as<AntLisp::Nil>(),
        AntLisp::Nil{}
    );
}

void test_less_multiple_symbol_true() {
    auto less = AntLisp::Builtin::Less();
    auto args = AntLisp::Arguments{};
    args.push_back(AntLisp::Cell::symbol('a'));
    args.push_back(AntLisp::Cell::symbol('b'));
    args.push_back(AntLisp::Cell::symbol('c'));
    args.push_back(AntLisp::Cell::symbol('d'));
    args.push_back(AntLisp::Cell::symbol('e'));
    args.push_back(AntLisp::Cell::symbol('y'));
    args.push_back(AntLisp::Cell::symbol('z'));
    UT_ASSERT(
        not less.instantCall(
            std::move(args)
        ).is<AntLisp::Nil>()
    );
}

void test_less_multiple_int_nil() {
    auto less = AntLisp::Builtin::Less();
    auto args = AntLisp::Arguments{};
    args.push_back(AntLisp::Cell::integer(10));
    args.push_back(AntLisp::Cell::integer(20));
    args.push_back(AntLisp::Cell::integer(30));
    args.push_back(AntLisp::Cell::integer(30));
    args.push_back(AntLisp::Cell::integer(40));
    UT_ASSERT_EQUAL(
        less.instantCall(
            std::move(args)
        ).as<AntLisp::Nil>(),
        AntLisp::Nil{}
    );
}

void test_less_multiple_symbol_nil() {
    auto less = AntLisp::Builtin::Less();
    auto args = AntLisp::Arguments{};
    args.push_back(AntLisp::Cell::symbol('1'));
    args.push_back(AntLisp::Cell::symbol('2'));
    args.push_back(AntLisp::Cell::symbol('3'));
    args.push_back(AntLisp::Cell::symbol('3'));
    args.push_back(AntLisp::Cell::symbol('4'));
    UT_ASSERT_EQUAL(
        less.instantCall(
            std::move(args)
        ).as<AntLisp::Nil>(),
        AntLisp::Nil{}
    );
}

UT_LIST(
    RUN_TEST(test_less_int_nil);
    RUN_TEST(test_less_int_true);
    RUN_TEST(test_less_real_nil);
    RUN_TEST(test_less_real_true);
    RUN_TEST(test_less_int_and_real_true);
    RUN_TEST(test_less_neg_real_and_neg_int);
    //RUN_TEST(test_less_str_true);
    //RUN_TEST(test_less_str_nil);
    RUN_TEST(test_less_symbol_true);
    RUN_TEST(test_less_symbol_nil);
    RUN_TEST(test_less_multiple_symbol_true);
    RUN_TEST(test_less_multiple_int_nil);
    RUN_TEST(test_less_multiple_symbol_nil);
);
