#include "error.h"

#include <antlisp/lib/cell/cell.h>

#include <antlisp/lib/test/ut.h>

#include <iostream>


void test_antlisp_simple_to_string() {
    auto value = AntLisp::Cell::ext<AntLisp::LispError>(
        AntLisp::LispError("Some nice error message")
    );
    std::cerr << value.toString() << "\n";
    UT_ASSERT_EQUAL(
        value.toString(),
        std::string{"antlisp error(0.): \"Some nice error message\""}
    );
}

void test_antlisp_inherited_to_string() {
    auto err = AntLisp::LispError(
      AntLisp::LispError::CodeType{12},
      "Corrupt"
    );
    UT_ASSERT_EQUAL(
        err.message(),
        std::string{"antlisp error(12.): \"Corrupt\""}
    );
    auto nextErr = AntLisp::LispError::inherit(
      err,
      AntLisp::LispError::CodeType{35},
      "Incomplete"
    );
    UT_ASSERT_EQUAL(
        nextErr.message(),
        std::string{"antlisp error(35.12.): \"Incomplete\""}
    );
    auto nextNextErr = AntLisp::LispError::inherit(
      nextErr,
      AntLisp::LispError::CodeType{833},
      "Failed"
    );
    UT_ASSERT_EQUAL(
        nextNextErr.message(),
        std::string{"antlisp error(833.35.12.): \"Failed\""}
    );
}

UT_LIST(
    RUN_TEST(test_antlisp_simple_to_string);
    RUN_TEST(test_antlisp_inherited_to_string);
);
