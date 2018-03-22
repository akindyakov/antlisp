#include "code_stream.h"

#include <antlisp/lib/test/ut.h>
#include <antlisp/lib/util/string.h>

#include <sstream>


void testSpaces() {
    std::istringstream in("  ( sum 1.23 ( * 2 3 ) )");
    auto code = AntLisp::InCodeStream(in);
    auto token = std::string();
    UT_ASSERT(
        not code.nextToken(token)
    );
    UT_ASSERT(
        not code.tryClose()
    );
    UT_ASSERT(
        code.good()
    );
    UT_ASSERT(
        code.tryOpen()
    );
    UT_ASSERT(
        code.nextToken(token)
    );
    UT_ASSERT_EQUAL(
        token, "sum"
    );
    UT_ASSERT_EQUAL(
        code.nextToken(), "1.23"
    );
    UT_ASSERT(
        not code.nextToken(token)
    );
    UT_ASSERT(
        not code.tryClose()
    );
    UT_ASSERT(
        code.good()
    );
    UT_ASSERT_EQUAL(
        code.getStat().parentheses, 1
    );
    UT_ASSERT(
        code.tryOpen()
    );
    UT_ASSERT_EQUAL(
        code.getStat().parentheses, 2
    );
    UT_ASSERT_EQUAL(
        code.nextToken(), "*"
    );
    UT_ASSERT_EQUAL(
        code.nextToken(), "2"
    );
    UT_ASSERT_EQUAL(
        code.nextToken(), "3"
    );
    UT_ASSERT(
        not code.nextToken(token)
    );
    UT_ASSERT_EQUAL(
        code.getStat().parentheses, 2
    );
    UT_ASSERT(
        code.tryClose()
    );
    UT_ASSERT_EQUAL(
        code.getStat().parentheses, 1
    );
    UT_ASSERT(
        code.tryClose()
    );
    UT_ASSERT_EQUAL(
        code.getStat().parentheses, 0
    );
    UT_ASSERT_EQUAL(
        code.getStat().lines, 1
    );
    UT_ASSERT_EQUAL(
        code.getStat().characters, 24
    );
}

void testMinorSpaces() {
    std::istringstream in("(sum 1.23(* 2 3)) ");
    auto code = AntLisp::InCodeStream(in);
    auto token = std::string();
    UT_ASSERT(
        not code.nextToken(token)
    );
    UT_ASSERT(
        not code.tryClose()
    );
    UT_ASSERT(
        code.good()
    );
    UT_ASSERT(
        code.tryOpen()
    );
    UT_ASSERT(
        code.nextToken(token)
    );
    UT_ASSERT_EQUAL(
        token, "sum"
    );
    UT_ASSERT_EQUAL(
        code.nextToken(), "1.23"
    );
    UT_ASSERT_EQUAL(
        code.getStat().parentheses, 1
    );
    UT_ASSERT(
        code.good()
    );
    UT_ASSERT(
        code.tryOpen()
    );
    UT_ASSERT_EQUAL(
        code.getStat().parentheses, 2
    );
    UT_ASSERT_EQUAL(
        code.nextToken(), "*"
    );
    UT_ASSERT_EQUAL(
        code.nextToken(), "2"
    );
    UT_ASSERT_EQUAL(
        code.nextToken(), "3"
    );
    UT_ASSERT(
        not code.nextToken(token)
    );
    UT_ASSERT_EQUAL(
        code.getStat().parentheses, 2
    );
    UT_ASSERT(
        code.good()
    );
    UT_ASSERT(
        not code.tryOpen()
    );
    UT_ASSERT(
        code.tryClose()
    );
    UT_ASSERT_EQUAL(
        code.getStat().parentheses, 1
    );
    UT_ASSERT(
        code.tryClose()
    );
    UT_ASSERT_EQUAL(
        code.getStat().parentheses, 0
    );
    UT_ASSERT(
        not code.nextToken(token)
    );
    UT_ASSERT_EQUAL(
        code.getStat().lines, 1
    );
    UT_ASSERT_EQUAL(
        code.getStat().characters, 18
    );
    UT_ASSERT(
        not code.good()
    );
}

void test_reading_string_with_spaces() {
    std::istringstream in(R"(  "This may be" )");
    auto code = AntLisp::InCodeStream(in);
    auto token = std::string();
    UT_ASSERT(
        code.nextToken(token)
    );
    UT_ASSERT_EQUAL(
        token, R"("This may be")"
    );
}

void test_reading_string_with_escaping() {
    std::istringstream in(R"(  "This is a \"neverland\"" )");
    auto code = AntLisp::InCodeStream(in);
    auto token = std::string();
    UT_ASSERT(
        code.nextToken(token)
    );
    UT_ASSERT_EQUAL(
        token, R"("This is a "neverland"")"
    );
}

void test_reading_string_with_newline_char() {
    std::istringstream in(R"(  "I must
not fear" )");
    auto code = AntLisp::InCodeStream(in);
    auto token = std::string();
    UT_ASSERT(
        code.nextToken(token)
    );
    UT_ASSERT_EQUAL(
        token, "\"I must\nnot fear\""
    );
}

UT_LIST(
    RUN_TEST(testSpaces);
    RUN_TEST(testMinorSpaces);
    RUN_TEST(test_reading_string_with_spaces);
    RUN_TEST(test_reading_string_with_escaping);
    RUN_TEST(test_reading_string_with_newline_char);
);
