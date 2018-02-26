#include "parser.h"

#include <antlisp/src/function/stack_machine.h>

#include <antlisp/src/test/ut.h>

#include <iostream>


void testFullCycle() {
    std::istringstream in("  (+ 21 (* 2 3)) ");
    auto global = AntLisp::Namespace{
        {
            "+", AntLisp::Cell(std::make_shared<AntLisp::ExtSum>())
        },
        {
            "*", AntLisp::Cell(std::make_shared<AntLisp::ExtMultiplication>())
        }
    };
    auto native = AntLisp::parseCode(in, global);
    auto env = AntLisp::Environment(native);
    env.run();
    UT_ASSERT_EQUAL(
        env.ret.get<AntLisp::Integer>(),
        21 + (2 * 3)
    );
}

void test_parseCode_lambda_call() {
    auto global = AntLisp::Namespace{
        {"+", AntLisp::Cell(std::make_shared<AntLisp::ExtSum>())},
    };
    std::istringstream in("((lambda (x y) (+ x y 1)) 4 2)");
    auto native = AntLisp::parseCode(in, global);
    auto env = AntLisp::Environment(native);
    env.run();
    //**/ std::cerr << "ret: " << env.ret.get<AntLisp::Integer>() << '\n';
    UT_ASSERT_EQUAL(
        env.ret.get<AntLisp::Integer>(),
        4 + 2 + 1
    );
}

void test_parseCode_cond() {
    auto global = AntLisp::Namespace{
        {"+", AntLisp::Cell(std::make_shared<AntLisp::ExtSum>())},
    };
    std::istringstream in(R"antlisp-code(
    (cond
        (nil  (+ 1 2))
        (true (+ 2 3))
        ((+ 1 0) (+ 3 4))
    )
    )antlisp-code");
    auto native = AntLisp::parseCode(in, global);
    UT_ASSERT_EQUAL(native.fdef->consts.size(), 10);
    // FIXME: names is [+ + + +], uniq it for the sake of God
    UT_ASSERT_EQUAL(native.fdef->names.size(), 4);
    auto env = AntLisp::Environment(native);
    env.run();
    //**/ std::cerr << "ret: " << env.ret.toString() << '\n';
    UT_ASSERT_EQUAL(
        env.ret.get<AntLisp::Integer>(),
        2 + 3
    );
}


UT_LIST(
    testFullCycle();
    test_parseCode_lambda_call();
    test_parseCode_cond();
);
