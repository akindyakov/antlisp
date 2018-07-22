#include "parser.h"

#include <antlisp/lib/builtin/math.h>

#include <antlisp/lib/test/ut.h>

#include <sstream>


void test_parseCode() {
    auto global = AntLisp::Namespace{};
    global.emplace(
        "*",
        AntLisp::Cell::function(
            std::make_shared<AntLisp::Builtin::Multiplication>()
        )
    );
    global.emplace(
        "sum",
        AntLisp::Cell::function(
            std::make_shared<AntLisp::Builtin::Sum>()
        )
    );
    std::istringstream in("  (sum 1.23 (* 2 3)) ");
    auto native = AntLisp::parseCode(in, std::move(global));
    UT_ASSERT_EQUAL(native.fdef->consts.size(), 3);
    UT_ASSERT_EQUAL(native.fdef->names.size(), 2);
}

void test_parseCode_lambda() {
    auto global = AntLisp::Namespace{};
    global.emplace(
        "+",
        AntLisp::Cell::function(
            std::make_shared<AntLisp::Builtin::Sum>()
        )
    );
    std::istringstream in("(lambda (x) (+ x 1))");
    auto native = AntLisp::parseCode(in, std::move(global));
    UT_ASSERT_EQUAL(native.fdef->consts.size(), 1);
    UT_ASSERT_EQUAL(native.fdef->names.size(), 1);
}

void test_parseCode_wrong_labda__body_is_absent() {
    auto global = AntLisp::Namespace{};
    global.emplace(
        "+",
        AntLisp::Cell::function(
            std::make_shared<AntLisp::Builtin::Sum>()
        )
    );
    std::istringstream in("(lambda (x))");
    UT_ASSERT_EXCEPTION_TYPE(
        AntLisp::parseCode(in, std::move(global)),
        AntLisp::SyntaxError
    );
}

void test_parseCode_wrong_labda__args_and_body_is_absent() {
    auto global = AntLisp::Namespace{};
    global.emplace(
        "+",
        AntLisp::Cell::function(
            std::make_shared<AntLisp::Builtin::Sum>()
        )
    );
    std::istringstream in("(lambda)");
    UT_ASSERT_EXCEPTION_TYPE(
        AntLisp::parseCode(in, std::move(global)),
        AntLisp::SyntaxError
    );
}

void test_parseCode_cond() {
    auto global = AntLisp::Namespace{};
    global.emplace(
        "+",
        AntLisp::Cell::function(
            std::make_shared<AntLisp::Builtin::Sum>()
        )
    );
    std::istringstream in(R"antlisp-code(
    (cond
        (nil  ( + 1 2 ) )
        (t (+ 2 3))
        ((+ 1 0) (+ 3 4))
    )
    )antlisp-code");
    auto native = AntLisp::parseCode(in, std::move(global));
    UT_ASSERT_EQUAL(native.fdef->consts.size(), 10);
    // [+ + + +]
    UT_ASSERT_EQUAL(native.fdef->names.size(), 4);
}

void test_parseCode_let() {
    auto global = AntLisp::Namespace{};
    global.emplace(
        "+",
        AntLisp::Cell::function(
            std::make_shared<AntLisp::Builtin::Sum>()
        )
    );
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
        AntLisp::parseCode(in, std::move(global)),
        AntLisp::ParseError
    );
}

void test_parseCode_progn() {
    auto global = AntLisp::Namespace{};
    global.emplace(
        "+",
        AntLisp::Cell::function(
            std::make_shared<AntLisp::Builtin::Sum>()
        )
    );
    std::istringstream in(R"antlisp-code(
    (progn
        ( + 1 2 )
        (+  2  3)
        (  +  3  4)
    )
    )antlisp-code");
    auto native = AntLisp::parseCode(in, std::move(global));
    UT_ASSERT_EQUAL(native.fdef->consts.size(), 6);
    UT_ASSERT_EQUAL(native.fdef->names.size(), 3);
}

void test_parseCode_defun() {
    auto global = AntLisp::Namespace{};
    global.emplace(
        "+",
        AntLisp::Cell::function(
            std::make_shared<AntLisp::Builtin::Sum>()
        )
    );
    std::istringstream in(R"antlisp-code(
    (defun first (x)
        ( + x 2  )
    )
    )antlisp-code");
    auto native = AntLisp::parseCode(in, std::move(global));
    UT_ASSERT_EQUAL(native.fdef->consts.size(), 1);
    UT_ASSERT_EQUAL(native.fdef->names.size(), 2);  // [+ first]
}

void test_parseCode_set() {
    auto global = AntLisp::Namespace{};
    global.emplace(
        "+",
        AntLisp::Cell::function(
            std::make_shared<AntLisp::Builtin::Sum>()
        )
    );
    std::istringstream in(R"antlisp-code(
    (progn
      (set xx 1)
      (set xy (+ xx 2))
    )
    )antlisp-code");
    auto native = AntLisp::parseCode(in, std::move(global));
    UT_ASSERT_EQUAL(native.fdef->consts.size(), 2);
    UT_ASSERT_EQUAL(native.fdef->names.size(), 4);  // [xx xy + xx]
}

void test_ParserOptions_defautls() {
    auto opts = AntLisp::ParserOptions{};
    UT_ASSERT(opts.test<AntLisp::Keywords::Defun>());
    UT_ASSERT(opts.test<AntLisp::Keywords::Lambda>());
    UT_ASSERT(opts.test<AntLisp::Keywords::Let>());
    UT_ASSERT(opts.test<AntLisp::Keywords::Set>());
    UT_ASSERT(opts.test<AntLisp::Keywords::Cond>());
    UT_ASSERT(opts.test<AntLisp::Keywords::Progn>());
    UT_ASSERT(opts.test<AntLisp::Keywords::AllKeywords>());
}

void test_ParserOptions_unset_all() {
    auto opts = AntLisp::ParserOptions{};
    opts.unset<AntLisp::Keywords::AllKeywords>();
    UT_ASSERT(not opts.test<AntLisp::Keywords::Defun>());
    UT_ASSERT(not opts.test<AntLisp::Keywords::Lambda>());
    UT_ASSERT(not opts.test<AntLisp::Keywords::Let>());
    UT_ASSERT(not opts.test<AntLisp::Keywords::Set>());
    UT_ASSERT(not opts.test<AntLisp::Keywords::Cond>());
    UT_ASSERT(not opts.test<AntLisp::Keywords::Progn>());
    UT_ASSERT(not opts.test<AntLisp::Keywords::AllKeywords>());
}

void test_ParserOptions_defun() {
    auto opts = AntLisp::ParserOptions{};
    opts.unset<AntLisp::Keywords::AllKeywords>();
    UT_ASSERT(not opts.test<AntLisp::Keywords::Defun>());
    opts.set<AntLisp::Keywords::Defun>();
    UT_ASSERT(opts.test<AntLisp::Keywords::Defun>());
    UT_ASSERT(not opts.test<AntLisp::Keywords::Lambda>());
    UT_ASSERT(not opts.test<AntLisp::Keywords::Let>());
    UT_ASSERT(not opts.test<AntLisp::Keywords::Set>());
    UT_ASSERT(not opts.test<AntLisp::Keywords::Cond>());
    UT_ASSERT(not opts.test<AntLisp::Keywords::Progn>());
    UT_ASSERT(not opts.test<AntLisp::Keywords::AllKeywords>());
}

void test_ParserOptions_lambda() {
    auto opts = AntLisp::ParserOptions{};
    opts.unset<AntLisp::Keywords::AllKeywords>();
    UT_ASSERT(not opts.test<AntLisp::Keywords::Lambda>());
    opts.set<AntLisp::Keywords::Lambda>();
    UT_ASSERT(not opts.test<AntLisp::Keywords::Defun>());
    UT_ASSERT(opts.test<AntLisp::Keywords::Lambda>());
    UT_ASSERT(not opts.test<AntLisp::Keywords::Let>());
    UT_ASSERT(not opts.test<AntLisp::Keywords::Set>());
    UT_ASSERT(not opts.test<AntLisp::Keywords::Cond>());
    UT_ASSERT(not opts.test<AntLisp::Keywords::Progn>());
}

void test_ParserOptions_let() {
    auto opts = AntLisp::ParserOptions{};
    opts.unset<AntLisp::Keywords::AllKeywords>();
    UT_ASSERT(not opts.test<AntLisp::Keywords::Let>());
    opts.set<AntLisp::Keywords::Let>();
    UT_ASSERT(not opts.test<AntLisp::Keywords::Defun>());
    UT_ASSERT(not opts.test<AntLisp::Keywords::Lambda>());
    UT_ASSERT(opts.test<AntLisp::Keywords::Let>());
    UT_ASSERT(not opts.test<AntLisp::Keywords::Set>());
    UT_ASSERT(not opts.test<AntLisp::Keywords::Cond>());
    UT_ASSERT(not opts.test<AntLisp::Keywords::Progn>());
}

void test_ParserOptions_cond() {
    auto opts = AntLisp::ParserOptions{};
    opts.unset<AntLisp::Keywords::AllKeywords>();
    UT_ASSERT(not opts.test<AntLisp::Keywords::Cond>());
    opts.set<AntLisp::Keywords::Cond>();
    UT_ASSERT(not opts.test<AntLisp::Keywords::Defun>());
    UT_ASSERT(not opts.test<AntLisp::Keywords::Lambda>());
    UT_ASSERT(not opts.test<AntLisp::Keywords::Let>());
    UT_ASSERT(not opts.test<AntLisp::Keywords::Set>());
    UT_ASSERT(opts.test<AntLisp::Keywords::Cond>());
    UT_ASSERT(not opts.test<AntLisp::Keywords::Progn>());
}

void test_parseCode_switched_off_defun() {
    auto global = AntLisp::Namespace{};
    global.emplace(
        "+",
        AntLisp::Cell::function(
            std::make_shared<AntLisp::Builtin::Sum>()
        )
    );
    std::istringstream in(R"antlisp-code(
    (defun first (x)
        (+ x 1)
    )
    )antlisp-code");

    auto opts = AntLisp::ParserOptions{};
    opts.set<AntLisp::Keywords::AllKeywords>();
    opts.unset<AntLisp::Keywords::Defun>();
    UT_ASSERT(opts.test<AntLisp::Keywords::Cond>());
    UT_ASSERT(not opts.test<AntLisp::Keywords::Defun>());

    UT_ASSERT_EXCEPTION_TYPE(
        AntLisp::parseCode(in, std::move(global), opts),
        AntLisp::ParseError
    );
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
    RUN_TEST(test_ParserOptions_defautls);
    RUN_TEST(test_ParserOptions_unset_all);
    RUN_TEST(test_ParserOptions_defun);
    RUN_TEST(test_ParserOptions_lambda);
    RUN_TEST(test_ParserOptions_let);
    RUN_TEST(test_ParserOptions_cond);
    RUN_TEST(test_parseCode_switched_off_defun);
);
