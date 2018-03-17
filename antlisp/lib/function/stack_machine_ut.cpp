#include "stack_machine.h"

#include <antlisp/lib/test/ut.h>

#include <iostream>


void testFullCycle() {
    auto global = AntLisp::Namespace{
        {
            "+", AntLisp::Cell(std::make_shared<AntLisp::ExtSum>())
        },
        {
            "*", AntLisp::Cell(std::make_shared<AntLisp::ExtMultiplication>())
        },
        {
            "first", AntLisp::Cell::integer(12)
        },
        {
            "second", AntLisp::Cell::integer(13)
        },
        {
            "third", AntLisp::Cell::integer(14)
        },
    };
    auto fdef = std::make_shared<AntLisp::NativeFunctionDefinition>();
    fdef->names.push_back("*");
    fdef->names.push_back("+");
    fdef->names.push_back("first");
    fdef->names.push_back("second");
    fdef->names.push_back("third");
    fdef->operations.push_back(
        AntLisp::NativeFunctionDefinition::Step(
            AntLisp::NativeFunctionDefinition::GetLocal,
            0
        )
    );
    fdef->operations.push_back(
        AntLisp::NativeFunctionDefinition::Step(
            AntLisp::NativeFunctionDefinition::GetLocal,
            1
        )
    );
    fdef->operations.push_back(
        AntLisp::NativeFunctionDefinition::Step(
            AntLisp::NativeFunctionDefinition::GetLocal,
            2
        )
    );
    fdef->operations.push_back(
        AntLisp::NativeFunctionDefinition::Step(
            AntLisp::NativeFunctionDefinition::GetLocal,
            3
        )
    );
    fdef->operations.push_back(
        AntLisp::NativeFunctionDefinition::Step(
            AntLisp::NativeFunctionDefinition::RunFunction,
            2
        )
    );
    fdef->operations.push_back(
        AntLisp::NativeFunctionDefinition::Step(
            AntLisp::NativeFunctionDefinition::GetLocal,
            4
        )
    );
    fdef->operations.push_back(
        AntLisp::NativeFunctionDefinition::Step(
            AntLisp::NativeFunctionDefinition::RunFunction,
            2
        )
    );
    auto env = AntLisp::Environment(
        AntLisp::NativeFunction(
            std::move(fdef), 0, global, "this"
        )
    );
    env.run();
    UT_ASSERT_EQUAL(
        env.ret.get<AntLisp::Integer>(),
        (12 + 13) * 14
    );
}

void testLambdaFunction() {
    auto global = AntLisp::Namespace{
        {
            "+", AntLisp::Cell(std::make_shared<AntLisp::ExtSum>())
        },
        {
            "global_first", AntLisp::Cell::integer(2209)
        },
        {
            "global_second", AntLisp::Cell::integer(1043)
        }
    };
    auto corefdef = std::make_shared<AntLisp::NativeFunctionDefinition>();
    corefdef->names.push_back("+");
    corefdef->names.push_back("local_first");
    corefdef->names.push_back("local_second");
    corefdef->names.push_back("local_third");
    // get the native function def
    corefdef->operations.push_back(
        AntLisp::NativeFunctionDefinition::Step(
            AntLisp::NativeFunctionDefinition::GetLocal,
            0
        )
    );
    // get 0 arg from local
    corefdef->operations.push_back(
        AntLisp::NativeFunctionDefinition::Step(
            AntLisp::NativeFunctionDefinition::GetLocal,
            1
        )
    );
    // get 1 arg from local
    corefdef->operations.push_back(
        AntLisp::NativeFunctionDefinition::Step(
            AntLisp::NativeFunctionDefinition::GetLocal,
            2
        )
    );
    // get 2 arg from local
    corefdef->operations.push_back(
        AntLisp::NativeFunctionDefinition::Step(
            AntLisp::NativeFunctionDefinition::GetLocal,
            3
        )
    );
    corefdef->operations.push_back(
        AntLisp::NativeFunctionDefinition::Step(
            AntLisp::NativeFunctionDefinition::RunFunction,
            3  // number of arguments
        )
    );
    auto coreLocal = global;
    coreLocal.emplace(
        AntLisp::TVarName{"local_first"}, AntLisp::Cell::integer(81)
    );
    auto nativeCore = AntLisp::NativeFunction(
        std::move(corefdef), 0, coreLocal, "this"
    );
    auto firstLambda = std::make_shared<AntLisp::LambdaFunction>(
        std::move(nativeCore),
        std::vector<AntLisp::TVarName>{
            AntLisp::TVarName{"local_second"},
            AntLisp::TVarName{"local_third"},
        }
    );
    global.insert(
        std::make_pair(
            "second_function", AntLisp::Cell::function(firstLambda)
        )
    );
    auto gDef = std::make_shared<AntLisp::NativeFunctionDefinition>();
    gDef->names.push_back("second_function");
    gDef->names.push_back("global_first");
    gDef->names.push_back("global_second");
    // get the native function def
    gDef->operations.push_back(
        AntLisp::NativeFunctionDefinition::Step(
            AntLisp::NativeFunctionDefinition::GetLocal,
            0
        )
    );
    // get 0 arg from global
    gDef->operations.push_back(
        AntLisp::NativeFunctionDefinition::Step(
            AntLisp::NativeFunctionDefinition::GetLocal,
            1
        )
    );
    // get 1 arg from global
    gDef->operations.push_back(
        AntLisp::NativeFunctionDefinition::Step(
            AntLisp::NativeFunctionDefinition::GetLocal,
            2
        )
    );
    // call first lambda
    gDef->operations.push_back(
        AntLisp::NativeFunctionDefinition::Step(
            AntLisp::NativeFunctionDefinition::RunFunction,
            2  // number of arguments
        )
    );
    // call core function
    gDef->operations.push_back(
        AntLisp::NativeFunctionDefinition::Step(
            AntLisp::NativeFunctionDefinition::RunFunction,
            0  // number of arguments
        )
    );
    auto env = AntLisp::Environment(
        AntLisp::NativeFunction{
            std::move(gDef), 0, global, "this"
        }
    );
    env.run();
    UT_ASSERT_EQUAL(
        env.ret.get<AntLisp::Integer>(),
        (81 + 1043 + 2209)
    );
}

UT_LIST(
    RUN_TEST(testFullCycle);
    RUN_TEST(testLambdaFunction);
);
