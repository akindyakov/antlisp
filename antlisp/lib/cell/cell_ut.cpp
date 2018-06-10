#include "cell.h"

#include <antlisp/lib/test/ut.h>

#include <iostream>
#include <string>
#include <vector>

void nilEqualTest() {
    UT_ASSERT_EQUAL(
        AntLisp::Nil{}, AntLisp::Nil{}
    );
    const auto cValue1 = AntLisp::Nil{};
    const auto cValue2 = AntLisp::Nil{};
    UT_ASSERT_EQUAL(cValue1, cValue1);
    UT_ASSERT_EQUAL(cValue1, cValue2);
}

template<typename T>
void checkTagTempl() {
    auto v = AntLisp::Cell::ext<T>(T{});
    auto first = v.typeId();
    auto second = AntLisp::Cell::typeIdOf<T>();
    UT_ASSERT_EQUAL(first, second);
}

void cellCheckTypeTag() {
    checkTagTempl<AntLisp::Nil>();
    checkTagTempl<AntLisp::Integer>();
    checkTagTempl<AntLisp::Float>();
    checkTagTempl<AntLisp::Symbol>();
    checkTagTempl<AntLisp::FunctionPtr>();

    checkTagTempl<short>();
    checkTagTempl<short int>();
    checkTagTempl<signed short>();
    checkTagTempl<signed short int>();
    checkTagTempl<unsigned short>();
    checkTagTempl<unsigned short int>();
    checkTagTempl<unsigned short int>();
    checkTagTempl<int>();
    checkTagTempl<signed>();
    checkTagTempl<signed int>();
    checkTagTempl<unsigned>();
    checkTagTempl<unsigned int>();
    checkTagTempl<unsigned int>();
    checkTagTempl<long>();
    checkTagTempl<long int>();
    checkTagTempl<signed long>();
    checkTagTempl<signed long int>();
    checkTagTempl<unsigned long>();
    checkTagTempl<unsigned long int>();
    checkTagTempl<unsigned long int>();
    checkTagTempl<long long>();
    checkTagTempl<long long int>();
    checkTagTempl<signed long long>();
    checkTagTempl<signed long long int>();
    checkTagTempl<unsigned long long>();
    checkTagTempl<unsigned long long int>();

    checkTagTempl<std::string>();
    checkTagTempl<std::vector<std::string>>();
}

void cellGetError() {
    UT_ASSERT_EXCEPTION_TYPE(
        AntLisp::Cell::nil().as<AntLisp::Integer>(),
        AntLisp::Cell::BadGetError
    );
}

void testCell_cast_nil() {
    UT_ASSERT(
        AntLisp::Cell::nil().cast<AntLisp::Nil>().is<AntLisp::Nil>()
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
        AntLisp::Cell::integer(1334)
            .cast<AntLisp::Integer>()
            .as<AntLisp::Integer>(),
        AntLisp::Integer{1334}
    );
    UT_ASSERT_EQUAL(
        AntLisp::Cell::integer(1334)
            .cast<AntLisp::Float>()
            .as<AntLisp::Float>(),
        AntLisp::Float{1334}
    );
    UT_ASSERT_EQUAL(
        AntLisp::Cell::integer(64)
            .cast<AntLisp::Symbol>()
            .as<AntLisp::Symbol>(),
        AntLisp::Symbol{'@'}
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
        AntLisp::Cell::real(1339.2)
            .cast<AntLisp::Integer>()
            .as<AntLisp::Integer>(),
        AntLisp::Integer{1339}
    );
    UT_ASSERT_EQUAL(
        AntLisp::Cell::real(1338.1)
            .cast<AntLisp::Float>()
            .as<AntLisp::Float>(),
        AntLisp::Float{1338.1}
    );
    UT_ASSERT_EXCEPTION_TYPE(
        AntLisp::Cell::real(1.0).cast<AntLisp::Symbol>(),
        AntLisp::Cell::BadGetError
    );
    UT_ASSERT_EXCEPTION_TYPE(
        AntLisp::Cell::real(4.0).cast<AntLisp::FunctionPtr>(),
        AntLisp::Cell::BadGetError
    );
}

void testCell_cast_symbol() {
    UT_ASSERT_EXCEPTION_TYPE(
        AntLisp::Cell::symbol('a').cast<AntLisp::Nil>(),
        AntLisp::Cell::BadGetError
    );
    UT_ASSERT_EQUAL(
        AntLisp::Cell::symbol('Y')
            .cast<AntLisp::Integer>()
            .as<AntLisp::Integer>(),
        AntLisp::Integer{89}
    );
    UT_ASSERT_EXCEPTION_TYPE(
        AntLisp::Cell::symbol('b').cast<AntLisp::Float>(),
        AntLisp::Cell::BadGetError
    );
    UT_ASSERT_EQUAL(
        AntLisp::Cell::symbol('W')
            .cast<AntLisp::Symbol>()
            .as<AntLisp::Symbol>(),
        AntLisp::Symbol{'W'}
    );
    UT_ASSERT_EXCEPTION_TYPE(
        AntLisp::Cell::symbol('e').cast<AntLisp::FunctionPtr>(),
        AntLisp::Cell::BadGetError
    );
}

void testCell_cast_function() {
    auto fnPtrCell = AntLisp::Cell::function(
        AntLisp::FunctionPtr{nullptr}
    );
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
}

void testCell_cast_copy_integer() {
    auto value = AntLisp::Cell::integer(167);
    auto cValue = value.copy();
    cValue.as<AntLisp::Integer>() += 12;
    UT_ASSERT_EQUAL(
        value.as<AntLisp::Integer>(),
        167
    );
    UT_ASSERT_EQUAL(
        cValue.as<AntLisp::Integer>(),
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
    RUN_TEST(testCell_cast_function);
    RUN_TEST(testCell_cast_copy_integer);
);
