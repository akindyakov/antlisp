#include "parser.h"

#include <antlisp/src/function/stack_machine.h>

#include <antlisp/src/test/ut.h>

#include <iostream>


void testFullCycle() {
    auto global = AntLisp::Namespace{};
    std::istringstream in("  (+ 21 (* 2 3)) ");
    auto body = AntLisp::parseCode(in, global);
    UT_ASSERT_EQUAL(body->consts.size(), 3);
    UT_ASSERT_EQUAL(body->names.size(), 2);

    AntLisp::Environment env;
    env.vars.insert(
        std::make_pair(
            "+", AntLisp::Cell(std::make_shared<AntLisp::ExtSum>())
        )
    );
    env.vars.insert(
        std::make_pair(
            "*", AntLisp::Cell(std::make_shared<AntLisp::ExtPrint>())
        )
    );

    auto frame = AntLisp::LocalStack{};
    env.CallStack.push_back(
        AntLisp::FunctionCall(
            std::move(body),
            std::move(frame)
        )
    );
    while (AntLisp::FunctionDefinition::step(env)) {
    }
    UT_ASSERT_EQUAL(
        env.ret.get<AntLisp::Integer>(),
        21 + (2 * 3)
    );
}

UT_LIST(
    testFullCycle();
);
