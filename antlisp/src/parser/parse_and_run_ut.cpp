#include "parser.h"

#include <antlisp/src/function/stack_machine.h>

#include <antlisp/src/test/ut.h>

#include <iostream>


void testFullCycle() {
    std::istringstream in("  (+ 21 (* 2 3)) ");
    AntLisp::Environment env;
    env.vars.insert(
        std::make_pair(
            "+", AntLisp::Cell(std::make_shared<AntLisp::ExtSum>())
        )
    );
    env.vars.insert(
        std::make_pair(
            "*", AntLisp::Cell(std::make_shared<AntLisp::ExtMultiplication>())
        )
    );
    auto lambda = AntLisp::parseCode(in, env.vars);
    UT_ASSERT_EQUAL(lambda->core()->consts.size(), 3);
    UT_ASSERT_EQUAL(lambda->core()->names.size(), 2);
    env.CallStack.push_back(
        lambda->
            instantCall(AntLisp::Arguments{})
            .get<AntLisp::FunctionPtr>()->nativeCall(
                AntLisp::Arguments{}
            )
    );
    while (AntLisp::NativeFunctionDefinition::step(env)) {
    }
    UT_ASSERT_EQUAL(
        env.ret.get<AntLisp::Integer>(),
        21 + (2 * 3)
    );
}

UT_LIST(
    testFullCycle();
);
