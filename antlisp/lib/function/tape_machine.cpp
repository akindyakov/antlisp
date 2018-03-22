#include "tape_machine.h"

#include <antlisp/lib/util/string.h>

#include <algorithm>


namespace AntLisp {

void NativeFunctionDefinition::ApplyTailRecursionOptimization() {
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
    auto localIt = this->vars.find(name);
    if (localIt == this->vars.end()) {
        throw RuntimeError() << "There is no such local name " << Str::Quotes(name);
    }
    this->push(localIt->second);
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
    std::reverse(args.begin(), args.end());
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

void Environment::runFunctionImpl(NativeFunctionCall* call) {
    auto args = call->createArgs();
    auto cellToRun = call->pop();
    this->runCellWithArguments(call, cellToRun, std::move(args));
}

void Environment::runTailRecOptimizedFunctionImpl(
    NativeFunctionCall* call
) {
    auto args = call->createArgs();
    auto cellToRun = call->pop();
    this->popCall();
    this->runCellWithArguments(call, cellToRun, std::move(args));
}

void Environment::runCellWithArguments(
    NativeFunctionCall* call
    , Cell& cellToRun
    , Arguments args
) {
    if (cellToRun.is<FunctionPtr>()) {
        auto fnPtr = cellToRun.get<FunctionPtr>();
        if (fnPtr->isNative()) {
            this->pushCall(
                fnPtr->nativeCall(
                    std::move(args)
                )
            );
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
            << " Type error: cell (" << cellToRun.toString()
            << ") is not callable";
    }
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
                this->runFunctionImpl(call);
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
            case NativeFunctionDefinition::RunTailRecOptimizedFunction:
                if (this->CallStack.size() < 2) {
                    this->runFunctionImpl(call);
                } else {
                    this->runTailRecOptimizedFunctionImpl(call);
                }
                break;
        }
    } else {
        // save return value
        this->ret = call->pop();
        // preserve the last local vars (for import)
        this->vars = call->releaseLocals();
        // drop spent call
        this->popCall();
        // should we resume something from the stack?
        if (this->isStackEmpty()) {
            // done, nothing to do
            return false;
        }
        // resume it
        call = this->topCall();
        call->push(
            std::move(this->ret)
        );
        this->ret = Cell::nil();
    }
    return true;
}

}  // namespace AntLisp
