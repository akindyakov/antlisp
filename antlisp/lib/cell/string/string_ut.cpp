#include "string.h"

#include <antlisp/lib/cell/cell.h>

#include <antlisp/lib/test/ut.h>

#include <iostream>


void testCell_cast_copy_string() {
    auto value = AntLisp::Cell::ext<AntLisp::StringType>(
      "act"
    );
    auto copyValuePtr = value.copy();
    value.as<AntLisp::StringType>() += " local!";
    UT_ASSERT_EQUAL(
        value.as<AntLisp::StringType>(),
        AntLisp::StringType{"act local!"}
    );
    UT_ASSERT_EQUAL(
        copyValuePtr.as<AntLisp::StringType>(),
        AntLisp::StringType{"act"}
    );
}

void testCell_string_eq() {
    auto first = AntLisp::StringCell("act");
    auto second = AntLisp::StringCell("act");
    UT_ASSERT_EQUAL(
        first,
        second
    );
}

UT_LIST(
    RUN_TEST(testCell_cast_copy_string);
    RUN_TEST(testCell_string_eq);
);
