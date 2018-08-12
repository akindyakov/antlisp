#include "parser.h"

#include <antlisp/lib/function/tape_machine.h>
#include <antlisp/lib/builtin/math.h>

#include <antlisp/lib/test/ut.h>

#include <iostream>


void testFullCycle() {
    std::istringstream in("  (+ 21 (* 2 3)) ");
    auto global = AntLisp::Namespace{};
    AntLisp::Builtin::allMathFunctions(global);
    auto native = AntLisp::parseCode(in, std::move(global));
    auto machine = AntLisp::TapeMachine(native);
    machine.run();
    UT_ASSERT_EQUAL(
        machine.ret().as<AntLisp::Integer>(),
        21 + (2 * 3)
    );
}

void test_parseCode_lambda_call() {
    auto global = AntLisp::Namespace{};
    AntLisp::Builtin::allMathFunctions(global);
    std::istringstream in("((lambda (x y) (+ x y 1)) 4 2)");
    auto native = AntLisp::parseCode(in, std::move(global));
    auto machine = AntLisp::TapeMachine(native);
    machine.run();
    UT_ASSERT_EQUAL(
        machine.ret().as<AntLisp::Integer>(),
        4 + 2 + 1
    );
}

void test_parseCode_lambda_recursive_call() {
    auto global = AntLisp::Namespace{};
    AntLisp::Builtin::allMathFunctions(global);
    std::istringstream in(R"(
    (
      (lambda (vAr)
        (cond
          ((= vAr 0) 0)
          (t (+ 1 (this (+ -1 vAr))))
        )
      )
      5
    )
    )");
    auto native = AntLisp::parseCode(in, std::move(global));
    auto machine = AntLisp::TapeMachine(native);
    machine.run();
    UT_ASSERT_EQUAL(
        machine.ret().as<AntLisp::Integer>(),
        5
    );
}

void test_parseCode_cond() {
    auto global = AntLisp::Namespace{};
    AntLisp::Builtin::allMathFunctions(global);
    std::istringstream in(R"antlisp-code(
    (cond
        (nil  (+ 1 2))
        (t (+ 2 3))
        ((+ 1 0) (+ 3 4))
    )
    )antlisp-code");
    auto native = AntLisp::parseCode(in, std::move(global));
    UT_ASSERT_EQUAL(native.fdef->consts.size(), 10);
    // [+ + + +]
    UT_ASSERT_EQUAL(native.fdef->names.size(), 4);
    auto machine = AntLisp::TapeMachine(native);
    machine.run();
    UT_ASSERT_EQUAL(
        machine.ret().as<AntLisp::Integer>(),
        2 + 3
    );
}

void test_parseCode_progn_simple() {
    auto global = AntLisp::Namespace{};
    AntLisp::Builtin::allMathFunctions(global);
    std::istringstream in(R"antlisp-code(
    (progn
      t
      nil
      (+ 4 44)
    )
    )antlisp-code");
    auto native = AntLisp::parseCode(in, std::move(global));
    auto machine = AntLisp::TapeMachine(native);
    machine.run();
    UT_ASSERT_EQUAL(
        machine.ret().as<AntLisp::Integer>(),
        4 + 44
    );
}

void test_parseCode_progn() {
    auto global = AntLisp::Namespace{};
    AntLisp::Builtin::allMathFunctions(global);
    std::istringstream in(R"antlisp-code(
    (
      (lambda (x y)
        (progn
          ( + 1 x )
          ( + x 3)
          ( + x y)
        )
      )
      12 13
    )
    )antlisp-code");
    auto native = AntLisp::parseCode(in, std::move(global));
    auto machine = AntLisp::TapeMachine(native);
    machine.run();
    UT_ASSERT_EQUAL(
        machine.ret().as<AntLisp::Integer>(),
        12 + 13
    );
}

void test_parseCode_defun_call() {
    auto global = AntLisp::Namespace{};
    AntLisp::Builtin::allMathFunctions(global);
    std::istringstream in(R"antlisp-code(
    (progn
        (defun ballad (of bull)
            (+ of bull 35)
        )
        (ballad 192 168)
    )
    )antlisp-code");
    auto native = AntLisp::parseCode(in, std::move(global));
    auto machine = AntLisp::TapeMachine(native);
    machine.run();
    UT_ASSERT_EQUAL(
        machine.ret().as<AntLisp::Integer>(),
        192 + 168 + 35
    );
}

void test_parseCode_defun_recursive_call() {
    auto global = AntLisp::Namespace{};
    AntLisp::Builtin::allMathFunctions(global);
    std::istringstream in(R"antlisp-code(
    (progn
      (defun ballad (n sum)
        (cond
          ((= n 0) sum)
          (t (ballad (+ n -1) (+ sum 2)))
        )
      )
      (ballad 5 0)
    )
    )antlisp-code");
    auto native = AntLisp::parseCode(in, std::move(global));
    auto machine = AntLisp::TapeMachine(native);
    machine.run();
    UT_ASSERT_EQUAL(
        machine.ret().as<AntLisp::Integer>(),
        5 * 2
    );
}

void test_parseCode_lambda_multi_call() {
    auto global = AntLisp::Namespace{};
    AntLisp::Builtin::allMathFunctions(global);
    std::istringstream in(R"code(
(
  (
    lambda (x y) (
      progn
        (+ x y 1)
        nil
        t
        (+ x y 1)
    )
  )
  4 2
)
)code");
    auto native = AntLisp::parseCode(in, std::move(global));
    auto machine = AntLisp::TapeMachine(native);
    machine.run();
    UT_ASSERT_EQUAL(
        machine.ret().as<AntLisp::Integer>(),
        4 + 2 + 1
    );
}


void test_parseCode_set() {
    auto global = AntLisp::Namespace{};
    AntLisp::Builtin::allMathFunctions(global);
    std::istringstream in(R"antlisp-code(
    (progn
      (set xx 181)
      (set xy (+ xx 39))
      xy
    )
    )antlisp-code");
    auto native = AntLisp::parseCode(in, std::move(global));
    auto machine = AntLisp::TapeMachine(native);
    machine.run();
    UT_ASSERT_EQUAL(
        machine.ret().as<AntLisp::Integer>(),
        181 + 39
    );
}

void test_parseCode_multistate() {
    auto global = AntLisp::Namespace{};
    AntLisp::Builtin::allMathFunctions(global);
    std::istringstream in(R"antlisp-code(
    (set x 87)
    (set xy (+ x 250 250 242))
    xy
    )antlisp-code");
    auto native = AntLisp::parseCode(in, std::move(global));
    auto machine = AntLisp::TapeMachine(native);
    machine.run();
    UT_ASSERT_EQUAL(
        machine.ret().as<AntLisp::Integer>(),
        87 + 250 + 250 + 242
    );
}

UT_LIST(
    RUN_TEST(testFullCycle);
    RUN_TEST(test_parseCode_lambda_call);
    RUN_TEST(test_parseCode_lambda_recursive_call);
    RUN_TEST(test_parseCode_cond);
    RUN_TEST(test_parseCode_progn);
    RUN_TEST(test_parseCode_progn_simple);
    RUN_TEST(test_parseCode_lambda_multi_call);
    RUN_TEST(test_parseCode_defun_call);
    RUN_TEST(test_parseCode_defun_recursive_call);
    RUN_TEST(test_parseCode_set);
    RUN_TEST(test_parseCode_multistate);
);
