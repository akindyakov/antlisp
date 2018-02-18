#include "antlisp/src/stack_machine.h"

namespace AntLisp {

void FunctionDefinition::getGlobalName(
    const TVarName& name
) {
    auto pos = names.size();
    names.push_back(name);
    operations.emplace_back(
        FunctionDefinition::GetGlobal,
        pos
    );
}

FunctionCall::FunctionCall(
    FunctionDefinitionPtr fdef
    // positional args
    // named args - use tuple please for it
    , LocalStack args
)
    : function(
        std::move(fdef)
    )
    , localCallStack(
        std::move(args)
    )
{
    for (const auto& argName : this->function->argNames) {
        vars.emplace(argName, this->popCallStack());
    }
}

FunctionDefinition::EOperations FunctionCall::getOperation() const {
    return function->operations[runner].operation;
}

bool FunctionCall::next() noexcept {
    return ++runner < function->operations.size();
}

Cell FunctionCall::popCallStack() {
    auto value = std::move(
        localCallStack.back()
    );
    localCallStack.pop_back();
    return value;
}

void FunctionCall::pushCallStack(Cell cell) {
    localCallStack.push_back(
        std::move(cell)
    );
}

void FunctionCall::getConst() {
    // copy
    auto cell = function->consts[
        function->operations[runner].position
    ];
    this->pushCallStack(
        std::move(cell)
    );
}

void FunctionCall::getLocal() {
    // copy
    auto local = this->vars.at(
        this->function->names[
            this->function->operations[
                this->runner
            ].position
        ]
    );
    this->pushCallStack(
        std::move(local)
    );
}

void FunctionCall::setLocal() {
    const auto& name = function->names[
        this->function->operations[
            this->runner
        ].position
    ];
    this->vars[name] =  this->popCallStack();
}

void FunctionCall::getGlobal(const Namespace& global) {
    const auto& name = function->names[
        function->operations[runner].position
    ];
    // copy
    this->pushCallStack(
        global.at(name)
    );
}

void FunctionCall::setGlobal(Namespace& global) {
    const auto& name = function->names[
        function->operations[runner].position
    ];
    global[name] = this->popCallStack();
}

void FunctionCall::stackRewind() {
    localCallStack.resize(
        localCallStack.size() - function->operations[runner].position
    );
}

void FunctionCall::skip() {
    runner += function->operations[runner].position;
}

LocalStack FunctionCall::createArgs() {
    auto size = function->operations[runner].position;
    auto newFrame = LocalStack{};
    while (size--) {
        newFrame.push_back(
            popCallStack()
        );
    }
    return newFrame;
}

bool FunctionDefinition::step(Environment& env) {
    auto call = env.stackTop();
    switch (call->getOperation()) {
        case Nope:
            break;
        case GetConst:
            call->getConst();
            break;
        case GetLocal:
            call->getLocal();
            break;
        case SetLocal:
            call->setLocal();
            break;
        case GetGlobal:
            call->getGlobal(env.vars);
            break;
        case SetGlobal:
            call->setGlobal(env.vars);
            break;
        case RunFunction:
            {
                auto args = call->createArgs();
                auto toCall = call->popCallStack();
                if (toCall.is<FunctionDefinitionPtr>()) {
                    auto fdef = toCall.get<FunctionDefinitionPtr>();
                    call = env.stackPush(
                        FunctionCall(
                            std::move(fdef),
                            std::move(args)
                        )
                    );
                } else if (toCall.is<ExtFunctionPtr>()) {
                    auto extf = toCall.get<ExtFunctionPtr>();
                    call->pushCallStack(
                        extf->call(
                            std::move(args)
                        )
                    );
                } else {
                    throw Error()
                        << __FILE__ << ":" << __LINE__
                        << " " << toCall.toString();
                }
            }
            break;
        case StackRewind:
            call->stackRewind();
            break;
        case SkipIfTrue:
            {
                auto guard = call->popCallStack();
                if (guard.is<Nil>()) {
                    call->skip();
                }
            }
            break;
        case SkipIfFalse:
            {
                auto guard = call->popCallStack();
                if (!guard.is<Nil>()) {
                    call->skip();
                }
            }
            break;
    }
    if (!call->next()) {
        env.ret = std::move(
            call->popCallStack()
        );
        env.stackPop();
        if (env.isStackEmpty()) {
            return false;
        }
        if (env.ret.is<PostponedFunctionPtr>()) {
            auto ptr = env.ret.get<PostponedFunctionPtr>();
            call = env.stackPush(
                FunctionCall(
                    std::move(ptr->fdef),
                    std::move(ptr->args)
                )
            );
            call = env.stackTop();
        } else {
            call = env.stackTop();
            call->pushCallStack(
                std::move(env.ret)
            );
        }
        env.ret = Cell::nil();
    }
    return true;
}

}  // namespace AntLisp
