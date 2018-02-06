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

UT_LIST(
    nilEqualTest();
    cellCheckTypeTag();
    cellGetError();
);
