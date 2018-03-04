#include "stack_machine.h"

#include <antlisp/lib/util/string.h>


namespace AntLisp {

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
    --runner;
}

NativeFunctionDefinition::EOperations NativeFunctionCall::getOperation() const {
    return function->operations[runner].operation;
}

bool NativeFunctionCall::next() noexcept {
    return ++runner < function->operations.size();
}

Cell NativeFunctionCall::popCallStack() {
    return (
        localCallStack.size() == 0
        ? Cell::nil()
        : localCallStack.pop()
    );
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
    auto pos = this->function->operations[
        this->runner
    ].position;
    const auto& name = this->function->names[pos];
    // copy
    auto local = this->vars.at(name);
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
    this->vars[name] = this->popCallStack();
}

void NativeFunctionCall::stackRewind() {
    for (auto i = function->operations[runner].position; i != 0; --i) {
        this->popCallStack();
    }
}

void NativeFunctionCall::skipUntilMark() {
    auto mark = function->operations[runner].position;
    while (
        this->next()
    ) {
        auto step = function->operations[runner];
        if (
            step.operation == NativeFunctionDefinition::GuardMark
            && step.position == mark
        ) {
            break;
        }
    }
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

Namespace NativeFunctionCall::releaseLocals() {
    auto locals = std::move(vars);
    vars.clear();
    return locals;
}

bool Environment::step() {
    auto call = this->stackTop();
    if (call->next()) {
        switch (call->getOperation()) {
            case NativeFunctionDefinition::Nope:
                break;
            case NativeFunctionDefinition::GetConst:
                call->getConst();
                break;
            case NativeFunctionDefinition::GetLocal:
                call->getLocal();
                break;
            case NativeFunctionDefinition::SetLocal:
                call->setLocal();
                break;
            case NativeFunctionDefinition::RunFunction:
                {
                    auto args = call->createArgs();
                    auto toCall = call->popCallStack();
                    if (toCall.is<FunctionPtr>()) {
                        auto fnPtr = toCall.get<FunctionPtr>();
                        if (fnPtr->isNative()) {
                            this->stackPush(
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
                            << " Type error: cell (" << toCall.toString()
                            << ") is not callable";
                    }
                }
                break;
            case NativeFunctionDefinition::Skip:
                call->skipUntilMark();
                break;
            case NativeFunctionDefinition::SkipIfNil:
                {
                    auto guard = call->popCallStack();
                    if (guard.is<Nil>()) {
                        call->skipUntilMark();
                    }
                }
                break;
            case NativeFunctionDefinition::GuardMark:
                break;
            case NativeFunctionDefinition::LocalStackRewind:
                call->stackRewind();
                break;
        }
    } else {
        this->ret = call->popCallStack();
        this->vars = call->releaseLocals();
        this->stackPop();
        if (this->isStackEmpty()) {
            return false;
        }
        if (this->ret.is<FunctionPtr>()) {
            auto fnPtr = this->ret.get<FunctionPtr>();
            if (fnPtr->isPostponed()) {
                call = this->stackPush(
                    fnPtr->nativeCall(Arguments{})
                );
                return true;
            }
        }
        call = this->stackTop();
        call->pushCallStack(
            std::move(this->ret)
        );
        this->ret = Cell::nil();
    }
    return true;
}

}  // namespace AntLisp
