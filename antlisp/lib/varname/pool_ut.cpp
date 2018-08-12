#include "pool.h"

#include <antlisp/lib/test/ut.h>

void store_name() {
    auto names = NamePool{};
    UT_ASSERT_EQUAL(name.store(AntLisp::VarName("first")), 0);
    UT_ASSERT_EQUAL(name.store(AntLisp::VarName("second")), 1);
    UT_ASSERT_EQUAL(name.store(AntLisp::VarName("first")), 0);
    UT_ASSERT_EQUAL(name.store(AntLisp::VarName("third")), 2);
    UT_ASSERT_EQUAL(name.store(AntLisp::VarName("second")), 0);
    UT_ASSERT_EQUAL(name.store(AntLisp::VarName("first")), 0);
}

void store_name_link() {
    auto names = NamePool{};
    auto const name1 = AntLisp::VarName("first");
    auto const name3 = AntLisp::VarName("second");
    auto const name2 = AntLisp::VarName("third");

    auto nlink1 = AntLisp::VarNameLink("first");
    auto nlink3 = AntLisp::VarNameLink("second");
    auto nlink2 = AntLisp::VarNameLink("third");

    UT_ASSERT_EQUAL(nlink1, 0);
    UT_ASSERT_EQUAL(nlink2, 1);
    UT_ASSERT_EQUAL(nlink3, 2);

    UT_ASSERT_EQUAL(nlink1, 0);
    UT_ASSERT_EQUAL(nlink2, 1);
    UT_ASSERT_EQUAL(nlink3, 2);
}

UT_LIST(
    RUN_TEST(store_name);
    RUN_TEST(store_name_link);
);
