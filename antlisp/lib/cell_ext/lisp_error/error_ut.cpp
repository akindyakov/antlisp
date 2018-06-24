#include "error.h"

#include <antlisp/lib/cell/cell.h>

#include <antlisp/lib/test/ut.h>

#include <iostream>


void test_antlisp_simple_to_string() {
    auto value = AntLisp::Cell::ext<AntLisp::LispErrorPtr>(
        AntLisp::LispError::create(0, "Some nice error message")
    );
    UT_ASSERT_EQUAL(
        value.toString(),
        std::string{"(error: 0 \"Some nice error message\")\n"}
    );
}

void test_antlisp_inherited_to_string() {
    auto err = AntLisp::LispError::create(
        AntLisp::LispError::CodeType{12},
        "Corrupt"
    );
    UT_ASSERT_EQUAL(
        err->message(),
        std::string{"(error: 12 \"Corrupt\")\n"}
    );
    auto nextErr = AntLisp::LispError::inherit(
      err,
      AntLisp::LispError::CodeType{35},
      "Incomplete"
    );
    UT_ASSERT_EQUAL(
        nextErr->message(),
        std::string{R"lisp((error: 35 "Incomplete")
(error: 12 "Corrupt")
)lisp"}
    );
    auto nextNextErr = AntLisp::LispError::inherit(
      nextErr,
      AntLisp::LispError::CodeType{833},
      "Failed"
    );
    UT_ASSERT_EQUAL(
        nextNextErr->message(),
        std::string{R"lisp((error: 833 "Failed")
(error: 35 "Incomplete")
(error: 12 "Corrupt")
)lisp"}
    );
}

UT_LIST(
    RUN_TEST(test_antlisp_simple_to_string);
    RUN_TEST(test_antlisp_inherited_to_string);
);
