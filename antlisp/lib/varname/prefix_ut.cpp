#include "prefix.h"

#include <antlisp/lib/test/ut.h>

void creation() {
    auto prefix = AntLisp::VarNamePrefix("debit:credit:sum");
    UT_ASSERT_EQUAL(
        prefix.fullName(), "debit:credit:sum"
    );
    UT_ASSERT_EQUAL(
        prefix.firstName(), "debit"
    );
}

void next() {
    auto const prefix = AntLisp::VarNamePrefix("debit:credit:sum");
    UT_ASSERT_EQUAL(
        prefix.fullName(), "debit:credit:sum"
    );
    UT_ASSERT_EQUAL(
        prefix.firstName(), "debit"
    );
    auto const nextPrefix = prefix.next();
    UT_ASSERT_EQUAL(
        nextPrefix.fullName(), "credit:sum"
    );
    UT_ASSERT_EQUAL(
        nextPrefix.firstName(), "credit"
    );
    auto const nextNextPrefix = nextPrefix.next();
    UT_ASSERT_EQUAL(
        nextNextPrefix.fullName(), "sum"
    );
    UT_ASSERT_EQUAL(
        nextNextPrefix.firstName(), "sum"
    );
    auto const nextNextNextPrefix = nextNextPrefix.next();
    UT_ASSERT(
        not nextNextNextPrefix.isValid()
    );
}

UT_LIST(
    RUN_TEST(creation);
    RUN_TEST(next);
);
