#include "ut.h"

#include "parser.h"

#include <sstream>


void testExpressionReader() {
    std::istringstream in("  (sum 123 1.23 (* 2 3)) ");
    auto expr = AntLisp::ParenthesesExprReader::createFromStream(
        in
    );
    UT_ASSERT_EQUAL(
        expr.nextToken(), "sum"
    );
    UT_ASSERT_EQUAL(
        expr.nextToken(), "123"
    );
    UT_ASSERT_EQUAL(
        expr.nextToken(), "1.23"
    );
    UT_ASSERT_EXCEPTION_TYPE(
        expr.nextToken(), AntLisp::ParenthesesExprReader::Error
    );
    expr.ignore();
    UT_ASSERT_EQUAL(
        expr.count(), 2
    );
    UT_ASSERT_EQUAL(
        expr.nextToken(), "*"
    );
    UT_ASSERT_EQUAL(
        expr.nextToken(), "2"
    );
    UT_ASSERT_EQUAL(
        expr.nextToken(), "3"
    );
    UT_ASSERT_EQUAL(
        expr.count(), 2
    );
    expr.ignore();
    UT_ASSERT_EQUAL(
        expr.count(), 1
    );
    expr.ignore();
    UT_ASSERT(
        !expr.good()
    );
}

UT_LIST(
    testExpressionReader();
);
