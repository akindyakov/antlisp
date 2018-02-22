#include "antlisp/src/stack_machine.h"

namespace AntLisp {

void NativeFunctionDefinition::getGlobalName(
    const TVarName& name
) {
    auto pos = names.size();
    names.push_back(name);
    operations.emplace_back(
        NativeFunctionDefinition::GetGlobal,
        pos
    );
}

NativeFunctionCall::NativeFunctionCall(
    NativeFunctionDefinitionPtr fdef
    , Namespace predefinedVars
)
    : function(
        std::move(fdef)
    )
    , vars(
        std::move(predefinedVars)
    )
{
}

NativeFunctionDefinition::EOperations NativeFunctionCall::getOperation() const {
    return function->operations[runner].operation;
}

bool NativeFunctionCall::next() noexcept {
    return ++runner < function->operations.size();
}

Cell NativeFunctionCall::popCallStack() {
    return localCallStack.pop();
}

void NativeFunctionCall::pushCallStack(Cell cell) {
    localCallStack.push(
        std::move(cell)
    );
}

void NativeFunctionCall::getConst() {
    // copy
    auto cell = function->consts[
        function->operations[runner].position
    ];
    this->pushCallStack(
        std::move(cell)
    );
}

void NativeFunctionCall::getLocal() {
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

void NativeFunctionCall::setLocal() {
    const auto& name = function->names[
        this->function->operations[
            this->runner
        ].position
    ];
    this->vars[name] =  this->popCallStack();
}

void NativeFunctionCall::getGlobal(const Namespace& global) {
    const auto& name = function->names[
        function->operations[runner].position
    ];
    // copy
    this->pushCallStack(
        global.at(name)
    );
}

void NativeFunctionCall::setGlobal(Namespace& global) {
    const auto& name = function->names[
        function->operations[runner].position
    ];
    global[name] = this->popCallStack();
}

void NativeFunctionCall::stackRewind() {
    localCallStack.skip(
        function->operations[runner].position
    );
}

void NativeFunctionCall::skip() {
    runner += function->operations[runner].position;
}

Arguments NativeFunctionCall::createArgs() {
    auto size = function->operations[runner].position;
    auto args = Arguments{};
    while (size--) {
        args.push_back(
            popCallStack()
        );
    }
    return args;
}

bool NativeFunctionDefinition::step(Environment& env) {
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
                if (toCall.is<FunctionPtr>()) {
                    auto fnPtr = toCall.get<FunctionPtr>();
                    if (fnPtr->isNative()) {
                        call = env.stackPush(
                            fnPtr->nativeCall(args)
                        );
                    } else {
                        call->pushCallStack(
                            fnPtr->instantCall(
                                std::move(args)
                            )
                        );
                    }
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
        if (env.ret.is<FunctionPtr>()) {
            auto fnPtr = env.ret.get<FunctionPtr>();
            if (fnPtr->isPostponed()) {
                call = env.stackPush(
                    fnPtr->nativeCall(Arguments{})
                );
            }
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
