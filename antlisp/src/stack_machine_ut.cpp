#include "stack_machine.h"

#include "ut.h"

#include <iostream>


void testFullCycle() {
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
    env.vars.insert(
        std::make_pair(
            "first", AntLisp::Cell::integer(12)
        )
    );
    env.vars.insert(
        std::make_pair(
            "second", AntLisp::Cell::integer(13)
        )
    );
    env.vars.insert(
        std::make_pair(
            "third", AntLisp::Cell::integer(14)
        )
    );
    auto fdef = std::make_shared<AntLisp::FunctionDefinition>();
    auto frame = AntLisp::LocalFrame{};
    fdef->names.push_back("*");
    fdef->names.push_back("+");
    fdef->names.push_back("first");
    fdef->names.push_back("second");
    fdef->names.push_back("third");
    fdef->operations.push_back(
        AntLisp::FunctionDefinition::Step(
            AntLisp::FunctionDefinition::GetGlobal,
            0
        )
    );
    fdef->operations.push_back(
        AntLisp::FunctionDefinition::Step(
            AntLisp::FunctionDefinition::GetGlobal,
            1
        )
    );
    fdef->operations.push_back(
        AntLisp::FunctionDefinition::Step(
            AntLisp::FunctionDefinition::GetGlobal,
            2
        )
    );
    fdef->operations.push_back(
        AntLisp::FunctionDefinition::Step(
            AntLisp::FunctionDefinition::GetGlobal,
            3
        )
    );
    fdef->operations.push_back(
        AntLisp::FunctionDefinition::Step(
            AntLisp::FunctionDefinition::RunExternalFunction,
            2
        )
    );
    fdef->operations.push_back(
        AntLisp::FunctionDefinition::Step(
            AntLisp::FunctionDefinition::GetGlobal,
            4
        )
    );
    fdef->operations.push_back(
        AntLisp::FunctionDefinition::Step(
            AntLisp::FunctionDefinition::RunExternalFunction,
            2
        )
    );
    env.CallStack.push_back(
        AntLisp::FunctionCall(
            std::move(fdef),
            std::move(frame)
        )
    );
    while (AntLisp::FunctionDefinition::step(env)) {
    }
    UT_ASSERT_EQUAL(
        env.ret.get<AntLisp::Integer>(),
        (12 + 13) * 14
    );
}

UT_LIST(
    testFullCycle();
);
