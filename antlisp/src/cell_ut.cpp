#include "cell.h"

#include "ut.h"

#include <iostream>

void nilEqualTest() {
    UT_ASSERT_EQUAL(
        AntLisp::Cell::nil(), AntLisp::Cell::nil()
    );
    UT_ASSERT_EQUAL(
        AntLisp::Cell{AntLisp::Nil{}}, AntLisp::Cell::nil()
    );
    UT_ASSERT_EQUAL(
        AntLisp::Nil{}, AntLisp::Nil{}
    );
}

template<typename T>
void checkTagTempl() {
    auto v = AntLisp::Cell(T{});
    UT_ASSERT_EQUAL(
        static_cast<int>(v.tag()),
        static_cast<int>(AntLisp::Cell::tagOf<T>())
    );
}

void cellCheckTypeTag() {
    checkTagTempl<AntLisp::Nil>();
    checkTagTempl<AntLisp::Integer>();
    checkTagTempl<AntLisp::Float>();
    checkTagTempl<AntLisp::Symbol>();
    checkTagTempl<AntLisp::StringPtr>();
    checkTagTempl<AntLisp::ConsPtr>();
    checkTagTempl<AntLisp::FunctionDefinitionPtr>();
    checkTagTempl<AntLisp::ExtFunctionPtr>();
}

void cellGetError() {
    UT_ASSERT_EXCEPTION_TYPE(
        AntLisp::Cell::nil().get<AntLisp::Integer>(),
        AntLisp::Cell::BadGetError
    );
}

void test_tryFromString_number() {
    auto dst = AntLisp::tryFromString("12");
    UT_ASSERT(dst);
    UT_ASSERT_EQUAL(
        dst->get<AntLisp::Integer>(),
        12
    );
    dst = AntLisp::tryFromString("-511");
    UT_ASSERT(dst);
    UT_ASSERT_EQUAL(
        dst->get<AntLisp::Integer>(),
        -511
    );
    dst = AntLisp::tryFromString("-0911");
    UT_ASSERT(dst);
    UT_ASSERT_EQUAL(
        dst->get<AntLisp::Integer>(),
        -911
    );
    dst = AntLisp::tryFromString("0587");
    UT_ASSERT(dst);
    UT_ASSERT_EQUAL(
        dst->get<AntLisp::Integer>(),
        587
    );
    dst = AntLisp::tryFromString("1.01");
    UT_ASSERT(dst);
    UT_ASSERT(
        dst->is<AntLisp::Float>()
    );
    UT_ASSERT_EQUAL(
        static_cast<int>(dst->get<AntLisp::Float>()),
        1
    );
    dst = AntLisp::tryFromString("-2.01");
    UT_ASSERT(dst);
    UT_ASSERT(
        dst->is<AntLisp::Float>()
    );
    UT_ASSERT_EQUAL(
        static_cast<int>(dst->get<AntLisp::Float>()),
        -2
    );
    dst = AntLisp::tryFromString("0.02");
    UT_ASSERT(dst);
    UT_ASSERT(
        dst->is<AntLisp::Float>()
    );
    UT_ASSERT(
        dst->get<AntLisp::Float>() > 0.019
    );
    UT_ASSERT(
        dst->get<AntLisp::Float>() < 0.021
    );
    dst = AntLisp::tryFromString(".02");
    UT_ASSERT(dst);
    UT_ASSERT(
        dst->is<AntLisp::Float>()
    );
    UT_ASSERT(
        dst->get<AntLisp::Float>() > 0.019
    );
    UT_ASSERT(
        dst->get<AntLisp::Float>() < 0.021
    );
}

void test_tryFromString_string() {
    auto dst = AntLisp::tryFromString("\" abc \"");
    UT_ASSERT(dst);
    UT_ASSERT_EQUAL(
        *dst->get<AntLisp::StringPtr>().get(),
        " abc "
    );
}

void test_tryFromString_simple_char() {
    auto dst = AntLisp::tryFromString("#\\b");
    UT_ASSERT(dst);
    UT_ASSERT_EQUAL(
        dst->get<AntLisp::Symbol>(),
        'b'
    );
}

UT_LIST(
    nilEqualTest();
    cellCheckTypeTag();
    cellGetError();
    test_tryFromString_number();
    test_tryFromString_string();
    test_tryFromString_simple_char();
);
