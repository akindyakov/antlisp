#include "code_stream.h"

#include <antlisp/src/test/ut.h>

#include <sstream>


void testInCodeStream() {
    std::istringstream in("  (sum 1.23 (* 2 3)) ");
    auto code = AntLisp::InCodeStream(in);
    auto token = std::string();
    UT_ASSERT(
        !code.nextToken(token)
    );
    code.ignore(); // ignore '('
    UT_ASSERT(
        code.nextToken(token)
    );
    UT_ASSERT_EQUAL(
        token, "sum"
    );
    UT_ASSERT_EQUAL(
        code.nextToken(), "1.23"
    );
    UT_ASSERT_EXCEPTION_TYPE(
        code.nextToken(), AntLisp::InCodeStream::Error
    );
    code.ignore();
    UT_ASSERT_EQUAL(
        code.pCount(), 2
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
    code.ignore();
    code.ignore();
    UT_ASSERT_EQUAL(
        code.pCount(), 0
    );
}

UT_LIST(
    testInCodeStream();
);
