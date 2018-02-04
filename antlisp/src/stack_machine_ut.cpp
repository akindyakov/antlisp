#include "antlisp/src/stack_machine.h"

#include <iostream>


int main() {
    Environment env;
    env.vars.insert(
        std::make_pair(
            "+", Cell(std::make_shared<ExtSum>())
        )
    );
    env.vars.insert(
        std::make_pair(
            "*", Cell(std::make_shared<ExtPrint>())
        )
    );
    env.vars.insert(
        std::make_pair(
            "first", Cell(12)
        )
    );
    env.vars.insert(
        std::make_pair(
            "second", Cell(13)
        )
    );
    env.vars.insert(
        std::make_pair(
            "third", Cell(14)
        )
    );
    auto fdef = std::make_shared<FunctionDefinition>();
    auto frame = LocalFrame{};
    fdef->names.push_back("*");
    fdef->names.push_back("+");
    fdef->names.push_back("first");
    fdef->names.push_back("second");
    fdef->names.push_back("third");
    fdef->operations.push_back(
        FunctionDefinition::Step(
            FunctionDefinition::GetGlobal,
            0
        )
    );
    fdef->operations.push_back(
        FunctionDefinition::Step(
            FunctionDefinition::GetGlobal,
            1
        )
    );
    fdef->operations.push_back(
        FunctionDefinition::Step(
            FunctionDefinition::GetGlobal,
            2
        )
    );
    fdef->operations.push_back(
        FunctionDefinition::Step(
            FunctionDefinition::GetGlobal,
            3
        )
    );
    fdef->operations.push_back(
        FunctionDefinition::Step(
            FunctionDefinition::RunExternalFunction,
            2
        )
    );
    fdef->operations.push_back(
        FunctionDefinition::Step(
            FunctionDefinition::GetGlobal,
            4
        )
    );
    fdef->operations.push_back(
        FunctionDefinition::Step(
            FunctionDefinition::RunExternalFunction,
            2
        )
    );
    env.CallStack.push_back(
        FunctionCall(
            std::move(fdef),
            std::move(frame)
        )
    );
    while (FunctionDefinition::step(env)) {
    }
    if (boost::get<int>(env.ret) != 350) {
        std::cerr << boost::get<int>(env.ret) << '\n';
        throw std::exception();
    }
}
