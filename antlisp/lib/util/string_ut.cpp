#include "string.h"

#include <antlisp/lib/test/ut.h>


void testQuotes() {
    UT_ASSERT_EQUAL(
        AntLisp::Str::Quotes("where is your wings?"), "\"where is your wings?\""
    );
    UT_ASSERT_EQUAL(
        AntLisp::Str::Quotes("\""), "\"\"\""
    );
}

void testParentheses() {
    UT_ASSERT_EQUAL(
        AntLisp::Str::Parentheses("1 + 2 * 3"), "(1 + 2 * 3)"
    );
}

void testWrap() {
    UT_ASSERT_EQUAL(
        AntLisp::Str::Wrap(" / ", "prefix * 1", "suffix * 2"),
        "prefix * 1 / suffix * 2"
    );
}

UT_LIST(
    testQuotes();
    testParentheses();
    testWrap();
);
