#include "parser.h"

#include <antlisp/lib/function/builtin/math.h>

#include <antlisp/lib/test/ut.h>

#include <sstream>


void test_parseCode() {
    auto global = AntLisp::Namespace{
        {"*", AntLisp::Cell(std::make_shared<AntLisp::Builtin::Multiplication>())},
        {"sum", AntLisp::Cell(std::make_shared<AntLisp::Builtin::Sum>())},
    };
    std::istringstream in("  (sum 1.23 (* 2 3)) ");
    auto native = AntLisp::parseCode(in, global);
    UT_ASSERT_EQUAL(native.fdef->consts.size(), 3);
    UT_ASSERT_EQUAL(native.fdef->names.size(), 2);
}

void test_parseCode_lambda() {
    auto global = AntLisp::Namespace{
        {"+", AntLisp::Cell(std::make_shared<AntLisp::Builtin::Sum>())},
    };
    std::istringstream in("(lambda (x) (+ x 1))");
    auto native = AntLisp::parseCode(in, global);
    UT_ASSERT_EQUAL(native.fdef->consts.size(), 1);
    UT_ASSERT_EQUAL(native.fdef->names.size(), 1);
}

void test_parseCode_wrong_labda__body_is_absent() {
    auto global = AntLisp::Namespace{
        {"+", AntLisp::Cell(std::make_shared<AntLisp::Builtin::Sum>())},
    };
    std::istringstream in("(lambda (x))");
    UT_ASSERT_EXCEPTION_TYPE(
        AntLisp::parseCode(in, global),
        AntLisp::SyntaxError
    );
}

void test_parseCode_wrong_labda__args_and_body_is_absent() {
    auto global = AntLisp::Namespace{
        {"+", AntLisp::Cell(std::make_shared<AntLisp::Builtin::Sum>())},
    };
    std::istringstream in("(lambda)");
    UT_ASSERT_EXCEPTION_TYPE(
        AntLisp::parseCode(in, global),
        AntLisp::SyntaxError
    );
}

void test_parseCode_cond() {
    auto global = AntLisp::Namespace{
        {"+", AntLisp::Cell(std::make_shared<AntLisp::Builtin::Sum>())},
    };
    std::istringstream in(R"antlisp-code(
    (cond
        (nil  ( + 1 2 ) )
        (true (+ 2 3))
        ((+ 1 0) (+ 3 4))
    )
    )antlisp-code");
    auto native = AntLisp::parseCode(in, global);
    UT_ASSERT_EQUAL(native.fdef->consts.size(), 10);
    // FIXME: names is [+ + + +], uniq it for the sake of God
    UT_ASSERT_EQUAL(native.fdef->names.size(), 4);
}

void test_parseCode_let() {
    auto global = AntLisp::Namespace{
        {"+", AntLisp::Cell(std::make_shared<AntLisp::Builtin::Sum>())},
    };
    std::istringstream in(R"antlisp-code(
    (let
      (
        (y -7)
        z: 12
        {x 12}
      )
      (+ y 2 z)
    )
    )antlisp-code");
    UT_ASSERT_EXCEPTION_TYPE(
        AntLisp::parseCode(in, global),
        AntLisp::ParseError
    );
}

void test_parseCode_progn() {
    auto global = AntLisp::Namespace{
        {"+", AntLisp::Cell(std::make_shared<AntLisp::Builtin::Sum>())},
    };
    std::istringstream in(R"antlisp-code(
    (progn
        ( + 1 2 )
        (+  2  3)
        (  +  3  4)
    )
    )antlisp-code");
    auto native = AntLisp::parseCode(in, global);
    UT_ASSERT_EQUAL(native.fdef->consts.size(), 6);
    UT_ASSERT_EQUAL(native.fdef->names.size(), 3);
}

void test_parseCode_defun() {
    auto global = AntLisp::Namespace{
        {"+", AntLisp::Cell(std::make_shared<AntLisp::Builtin::Sum>())},
    };
    std::istringstream in(R"antlisp-code(
    (defun first (x)
        ( + x 2  )
    )
    )antlisp-code");
    auto native = AntLisp::parseCode(in, global);
    UT_ASSERT_EQUAL(native.fdef->consts.size(), 1);
    UT_ASSERT_EQUAL(native.fdef->names.size(), 2);  // [+ first]
}

void test_parseCode_set() {
    auto global = AntLisp::Namespace{
        {"+", AntLisp::Cell(std::make_shared<AntLisp::Builtin::Sum>())},
    };
    std::istringstream in(R"antlisp-code(
    (progn
      (set xx 1)
      (set xy (+ xx 2))
    )
    )antlisp-code");
    auto native = AntLisp::parseCode(in, global);
    UT_ASSERT_EQUAL(native.fdef->consts.size(), 2);
    UT_ASSERT_EQUAL(native.fdef->names.size(), 4);  // [xx xy + xx]
}

UT_LIST(
    RUN_TEST(test_parseCode);
    RUN_TEST(test_parseCode_lambda);
    RUN_TEST(test_parseCode_wrong_labda__body_is_absent);
    RUN_TEST(test_parseCode_wrong_labda__args_and_body_is_absent);
    RUN_TEST(test_parseCode_cond);
    RUN_TEST(test_parseCode_progn);
    RUN_TEST(test_parseCode_defun);
    RUN_TEST(test_parseCode_set);
);
