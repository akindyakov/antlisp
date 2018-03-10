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

Cell NativeFunctionCall::pop() {
    return (
        localCallStack.size() == 0
        ? Cell::nil()
        : localCallStack.pop()
    );
}

void NativeFunctionCall::push(Cell cell) {
    localCallStack.push(
        std::move(cell)
    );
}

void NativeFunctionCall::getConst() {
    // copy
    auto cell = function->consts[
        function->operations[runner].position
    ];
    this->push(
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
    this->push(
        std::move(local)
    );
}

void NativeFunctionCall::setLocal() {
    const auto& name = function->names[
        this->function->operations[
            this->runner
        ].position
    ];
    this->vars[name] = this->pop();
}

void NativeFunctionCall::stackRewind() {
    for (auto i = function->operations[runner].position; i != 0; --i) {
        this->pop();
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
            pop()
        );
    }
    return args;
}

Namespace NativeFunctionCall::releaseLocals() {
    auto locals = std::move(vars);
    vars.clear();
    return locals;
}

Environment::Environment(
    NativeFunctionCall toRun
)
{
    CallStack.push_back(
        std::move(toRun)
    );
}

Environment::Environment(
    const NativeFunction& toRun
)
    : Environment(
        toRun.nativeCall(
            Arguments{}
        )
    )
{
}

bool Environment::step() {
    auto call = this->topCall();
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
                    auto toCall = call->pop();
                    if (toCall.is<FunctionPtr>()) {
                        auto fnPtr = toCall.get<FunctionPtr>();
                        if (fnPtr->isNative()) {
                            auto nextCall = fnPtr->nativeCall(args);
                            if (call->isReadyToPostpone()) {
                                call->push(
                                    Cell{
                                        std::make_shared<PostponedFunction>(
                                            std::move(nextCall)
                                        )
                                    }
                                );
                            } else {
                                this->pushCall(
                                    std::move(nextCall)
                                );
                            }
                        } else {
                            call->push(
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
                    auto guard = call->pop();
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
        // save return value
        this->ret = call->pop();
        // preserve the last local vars (for import)
        this->vars = call->releaseLocals();
        // drop spent call
        this->popCall();
        // check if return value is postponed call
        if (this->ret.is<FunctionPtr>()) {
            auto fnPtr = this->ret.get<FunctionPtr>();
            if (fnPtr->isPostponed()) {
                call = this->pushCall(
                    fnPtr->nativeCall(Arguments{})
                );
                return true;
            }
        }
        if (this->isStackEmpty()) {
            // done, nothing to do
            return false;
        }
        call = this->topCall();
        call->push(
            std::move(this->ret)
        );
        this->ret = Cell::nil();
    }
    return true;
}

}  // namespace AntLisp
