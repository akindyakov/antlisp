#include "cell.h"

#include <antlisp/lib/test/ut.h>

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
    checkTagTempl<AntLisp::ExtTypePtr>();
    checkTagTempl<AntLisp::FunctionPtr>();
}

void cellGetError() {
    UT_ASSERT_EXCEPTION_TYPE(
        AntLisp::Cell::nil().get<AntLisp::Integer>(),
        AntLisp::Cell::BadGetError
    );
}

void testCell_cast_nil() {
    UT_ASSERT_EQUAL(
        AntLisp::Cell::nil().cast<AntLisp::Nil>(),
        AntLisp::Cell::nil()
    );
    UT_ASSERT_EXCEPTION_TYPE(
        AntLisp::Cell::nil().cast<AntLisp::Integer>(),
        AntLisp::Cell::BadGetError
    );
    UT_ASSERT_EXCEPTION_TYPE(
        AntLisp::Cell::nil().cast<AntLisp::Float>(),
        AntLisp::Cell::BadGetError
    );
    UT_ASSERT_EXCEPTION_TYPE(
        AntLisp::Cell::nil().cast<AntLisp::Symbol>(),
        AntLisp::Cell::BadGetError
    );
    UT_ASSERT_EXCEPTION_TYPE(
        AntLisp::Cell::nil().cast<AntLisp::ExtTypePtr>(),
        AntLisp::Cell::BadGetError
    );
    UT_ASSERT_EXCEPTION_TYPE(
        AntLisp::Cell::nil().cast<AntLisp::FunctionPtr>(),
        AntLisp::Cell::BadGetError
    );
}

void testCell_cast_integer() {
    UT_ASSERT_EXCEPTION_TYPE(
        AntLisp::Cell::integer(1).cast<AntLisp::Nil>(),
        AntLisp::Cell::BadGetError
    );
    UT_ASSERT_EQUAL(
        AntLisp::Cell::integer(1334).cast<AntLisp::Integer>(),
        AntLisp::Cell::integer(1334)
    );
    UT_ASSERT_EQUAL(
        AntLisp::Cell::integer(1334).cast<AntLisp::Float>(),
        AntLisp::Cell::real(1334)
    );
    UT_ASSERT_EQUAL(
        AntLisp::Cell::integer(64).cast<AntLisp::Symbol>(),
        AntLisp::Cell{'@'}
    );
    UT_ASSERT_EXCEPTION_TYPE(
        AntLisp::Cell::integer(3).cast<AntLisp::ExtTypePtr>(),
        AntLisp::Cell::BadGetError
    );
    UT_ASSERT_EXCEPTION_TYPE(
        AntLisp::Cell::integer(4).cast<AntLisp::FunctionPtr>(),
        AntLisp::Cell::BadGetError
    );
}

void testCell_cast_real() {
    UT_ASSERT_EXCEPTION_TYPE(
        AntLisp::Cell::real(0.0).cast<AntLisp::Nil>(),
        AntLisp::Cell::BadGetError
    );
    UT_ASSERT_EQUAL(
        AntLisp::Cell::real(1339.2).cast<AntLisp::Integer>(),
        AntLisp::Cell::integer(1339)
    );
    UT_ASSERT_EQUAL(
        AntLisp::Cell::real(1338.1).cast<AntLisp::Float>(),
        AntLisp::Cell::real(1338.1)
    );
    UT_ASSERT_EXCEPTION_TYPE(
        AntLisp::Cell::real(1.0).cast<AntLisp::Symbol>(),
        AntLisp::Cell::BadGetError
    );
    UT_ASSERT_EXCEPTION_TYPE(
        AntLisp::Cell::real(3.0).cast<AntLisp::ExtTypePtr>(),
        AntLisp::Cell::BadGetError
    );
    UT_ASSERT_EXCEPTION_TYPE(
        AntLisp::Cell::real(4.0).cast<AntLisp::FunctionPtr>(),
        AntLisp::Cell::BadGetError
    );
}

void testCell_cast_symbol() {
    UT_ASSERT_EXCEPTION_TYPE(
        AntLisp::Cell{'a'}.cast<AntLisp::Nil>(),
        AntLisp::Cell::BadGetError
    );
    UT_ASSERT_EQUAL(
        AntLisp::Cell{'Y'}.cast<AntLisp::Integer>(),
        AntLisp::Cell::integer(89)
    );
    UT_ASSERT_EXCEPTION_TYPE(
        AntLisp::Cell{'b'}.cast<AntLisp::Float>(),
        AntLisp::Cell::BadGetError
    );
    UT_ASSERT_EQUAL(
        AntLisp::Cell{'W'}.cast<AntLisp::Symbol>(),
        AntLisp::Cell{'W'}
    );
    UT_ASSERT_EXCEPTION_TYPE(
        AntLisp::Cell{'d'}.cast<AntLisp::ExtTypePtr>(),
        AntLisp::Cell::BadGetError
    );
    UT_ASSERT_EXCEPTION_TYPE(
        AntLisp::Cell{'e'}.cast<AntLisp::FunctionPtr>(),
        AntLisp::Cell::BadGetError
    );
}

void testCell_cast_ext() {
    auto extPtrCell = AntLisp::Cell{
        AntLisp::ExtTypePtr{nullptr}
    };
    UT_ASSERT_EXCEPTION_TYPE(
        extPtrCell.cast<AntLisp::Nil>(),
        AntLisp::Cell::BadGetError
    );
    UT_ASSERT_EXCEPTION_TYPE(
        extPtrCell.cast<AntLisp::Integer>(),
        AntLisp::Cell::BadGetError
    );
    UT_ASSERT_EXCEPTION_TYPE(
        extPtrCell.cast<AntLisp::Float>(),
        AntLisp::Cell::BadGetError
    );
    UT_ASSERT_EXCEPTION_TYPE(
        extPtrCell.cast<AntLisp::Symbol>(),
        AntLisp::Cell::BadGetError
    );
    UT_ASSERT_EXCEPTION_TYPE(
        extPtrCell.cast<AntLisp::FunctionPtr>(),
        AntLisp::Cell::BadGetError
    );
}

void testCell_cast_function() {
    auto fnPtrCell = AntLisp::Cell{
        AntLisp::FunctionPtr{nullptr}
    };
    UT_ASSERT_EXCEPTION_TYPE(
        fnPtrCell.cast<AntLisp::Nil>(),
        AntLisp::Cell::BadGetError
    );
    UT_ASSERT_EXCEPTION_TYPE(
        fnPtrCell.cast<AntLisp::Integer>(),
        AntLisp::Cell::BadGetError
    );
    UT_ASSERT_EXCEPTION_TYPE(
        fnPtrCell.cast<AntLisp::Float>(),
        AntLisp::Cell::BadGetError
    );
    UT_ASSERT_EXCEPTION_TYPE(
        fnPtrCell.cast<AntLisp::Symbol>(),
        AntLisp::Cell::BadGetError
    );
    UT_ASSERT_EXCEPTION_TYPE(
        fnPtrCell.cast<AntLisp::ExtTypePtr>(),
        AntLisp::Cell::BadGetError
    );
}

void testCell_cast_copy_integer() {
    auto value = AntLisp::Cell::integer(167);
    auto cValue = value.copy();
    cValue.get<AntLisp::Integer>() += 12;
    UT_ASSERT_EQUAL(
        value.get<AntLisp::Integer>(),
        167
    );
    UT_ASSERT_EQUAL(
        cValue.get<AntLisp::Integer>(),
        12 + 167
    );
}

UT_LIST(
    RUN_TEST(nilEqualTest);
    RUN_TEST(cellCheckTypeTag);
    RUN_TEST(cellGetError);
    RUN_TEST(testCell_cast_nil);
    RUN_TEST(testCell_cast_integer);
    RUN_TEST(testCell_cast_real);
    RUN_TEST(testCell_cast_symbol);
    RUN_TEST(testCell_cast_ext);
    RUN_TEST(testCell_cast_function);
    RUN_TEST(testCell_cast_copy_integer);
);
