#include "string.h"

#include <antlisp/lib/test/ut.h>

#include <iostream>


void testCell_cast_copy_string() {
    auto value = AntLisp::StringType{"act"};
    auto copyValuePtr = value.copy();
    value.summarize(
        AntLisp::Cell::ext<AntLisp::StringType>(
            " local!"
        )
    );
    UT_ASSERT_EQUAL(
        value.toString(),
        "\"act local!\""
    );
    UT_ASSERT_EQUAL(
        copyValuePtr->toString(),
        "\"act\""
    );
}

void testCell_string_eq() {
    auto first = AntLisp::Cell::ext<AntLisp::StringType>("act");
    auto second = AntLisp::Cell::ext<AntLisp::StringType>("act");
    UT_ASSERT_EQUAL(
        first,
        second
    );
}

UT_LIST(
    RUN_TEST(testCell_cast_copy_string);
    RUN_TEST(testCell_string_eq);
);
