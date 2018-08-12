#include "tape_machine.h"

#include <antlisp/lib/util/string.h>

#include <algorithm>


namespace AntLisp {

template<>
std::string CellType<FunctionPtr>::toString() const {
    return std::string{"_function_ptr_"};
}

void LocalStack::push(Cell value) {
    cells_.push_back(
        std::move(value)
    );
}

Cell LocalStack::pop() {
    auto value = std::move(
        cells_.back()
    );
    cells_.pop_back();
    return value;
}

std::size_t LocalStack::size() const noexcept {
    return cells_.size();
}

Namespace IFunction::parseArguments(
    Arguments& args
    , const std::vector<VarName>& names
    , const std::size_t argMaxNum
) {
    auto vars = Namespace{};
    std::size_t nameIndex = 0;
    auto rit = args.begin();
    while (rit != args.end() && nameIndex < argMaxNum) {
        const auto& argName = names[nameIndex];
        vars.set(argName, std::move(*rit));
        ++rit;
        ++nameIndex;
    }
    return vars;
}

NativeFunction::NativeFunction(
    NativeFunctionDefinitionPtr fdef_
    , std::size_t argnum_
    , Namespace closures
    , VarName selfName_
)
    : argnum(argnum_)
    , fdef(
        std::move(fdef_)
    )
    , closures_(
        std::move(closures)
    )
    , selfName(
        std::move(selfName_)
    )
{
    closures_.set(selfName, Cell::function(
        nullptr
    ));
}

bool NativeFunction::isNative() const {
    return true;
}

FunctionPtr NativeFunction::activate(
    Namespace vars
) const {
    vars.update(closures_);
    auto copy = std::make_shared<NativeFunction>(
        fdef, argnum, std::move(vars), selfName
    );
    // Add this ptr to local vars to make recursive call
    copy->closures_.set(
        selfName,
        Cell::function(
            copy
        )
    );
    // Not activated native function does not have 'this' variable
    return copy;
}

Cell NativeFunction::instantCall(
    Arguments /*args*/
) const {
    throw Error() << "Method 'nativeCall' is not valid for 'NativeFunction'";
    return Cell::nil();
}

NativeFunctionCall NativeFunction::nativeCall(
    Arguments args
) const {
    auto vars = IFunction::parseArguments(
        args,
        fdef->names,
        argnum
    );
    vars.update(closures_);
    return NativeFunctionCall(fdef, std::move(vars));
}

bool NativeFunction::hasName(
    const VarName& name
) const {
    auto last = fdef->names.cend();
    return (
        closures_.has(name)
        || last != std::find(fdef->names.cbegin(), last, name)
    );
}

std::string NativeFunction::toString() const {
    auto str = std::string{};
    str.append("native function: (\n");
    str.append("locals: (\n");
    str.append(closures_.toString());
    str.append(")\n");
    str.append(fdef->toString());
    str.append(")\n");
    return str;
}

LambdaFunction::LambdaFunction(
    NativeFunction native_
    , ArgNames names_
)
    : nativeFn(std::move(native_))
    , names(std::move(names_))
{
}

bool LambdaFunction::isNative() const {
    return false;
}

Cell LambdaFunction::instantCall(
    Arguments args
) const {
    auto nextClosures = IFunction::parseArguments(
        args,
        names,
        names.size()
    );
    return Cell::function(
        nativeFn.activate(std::move(nextClosures))
    );
}

NativeFunctionCall LambdaFunction::nativeCall(
    Arguments
) const {
    throw Error() << "Method 'nativeCall' is not valid for 'LambdaFunction'";
    return NativeFunctionCall{
        nullptr,
        Namespace{}
    };
}

NativeTape* LambdaFunction::core() {
    return nativeFn.fdef.get();
}

bool LambdaFunction::hasName(
    const VarName& name
) const {
    return (
        nativeFn.hasName(name)
        || names.end() != std::find(names.begin(), names.end(), name)
    );
}

std::string LambdaFunction::toString() const {
    std::ostringstream out;
    out << "lambda: ( ";
    for (const auto& name : names) {
        out << name << " ";
    }
    out << ")\n";
    out << nativeFn.toString();
    return out.str();
}

NativeFunctionCall::NativeFunctionCall(
    NativeFunctionDefinitionPtr fdef
    , Namespace predefinedVars
)
    : function_(
        std::move(fdef)
    )
    , vars_(
        std::move(predefinedVars)
    )
{
    --runner_;
}

NativeTape::EOperations NativeFunctionCall::getOperation() const {
    return function_->operations[runner_].operation;
}

bool NativeFunctionCall::next() noexcept {
    return ++runner_ < function_->operations.size();
}

Cell NativeFunctionCall::pop() {
    return (
        localCallStack_.size() == 0
        ? Cell::nil()
        : localCallStack_.pop()
    );
}

void NativeFunctionCall::push(Cell cell) {
    localCallStack_.push(
        std::move(cell)
    );
}

void NativeFunctionCall::getConst() {
    auto cell = function_->consts[
        function_->operations[runner_].operand
    ].copy();
    push(
        std::move(cell)
    );
}

void NativeFunctionCall::getLocal() {
    auto pos = function_->operations[
        runner_
    ].operand;
    const auto& name = function_->names[pos];
    push(
        vars_.get(name).copy()
    );
}

void NativeFunctionCall::setLocal() {
    const auto& name = function_->names[
        function_->operations[
            runner_
        ].operand
    ];
    vars_.set(name, pop());
}

void NativeFunctionCall::stackRewind() {
    for (auto i = function_->operations[runner_].operand; i != 0; --i) {
        pop();
    }
}

void NativeFunctionCall::skipUntilMark() {
    auto mark = function_->operations[runner_].operand;
    while (
        next()
    ) {
        const auto& step = function_->operations[runner_];
        if (
            step.operation == NativeTape::GuardMark
            && step.operand == mark
        ) {
            break;
        }
    }
}

Arguments NativeFunctionCall::createArgs() {
    auto size = function_->operations[runner_].operand;
    auto args = Arguments(size);
    for (auto& arg : args) {
        arg = pop();
    }
    std::reverse(args.begin(), args.end());
    return args;
}

Namespace NativeFunctionCall::releaseLocals() {
    return std::move(vars_);
}

TapeMachine::TapeMachine(
    const NativeFunction& toRun
)
{
    callStack_.push_back(
      toRun.nativeCall(
          Arguments{}
      )
    );
}

void TapeMachine::runFunctionImpl(NativeFunctionCall* call) {
    auto args = call->createArgs();
    auto cellToRun = call->pop();
    runCellWithArguments(call, cellToRun, std::move(args));
}

void TapeMachine::runTailRecOptimizedFunctionImpl(
    NativeFunctionCall* call
) {
    auto args = call->createArgs();
    auto cellToRun = call->pop();
    popCall();
    runCellWithArguments(topCall(), cellToRun, std::move(args));
}

void TapeMachine::runCellWithArguments(
    NativeFunctionCall* call
    , Cell& cellToRun
    , Arguments args
) {
    if (cellToRun.is<FunctionPtr>()) {
        auto fnPtr = cellToRun.as<FunctionPtr>();
        if (fnPtr->isNative()) {
            pushCall(
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

bool TapeMachine::isStackEmpty() const {
    return callStack_.empty();
}

NativeFunctionCall* TapeMachine::topCall() {
    return &callStack_.back();
}

NativeFunctionCall* TapeMachine::pushCall(
    NativeFunctionCall frame
) {
    callStack_.push_back(
        std::move(frame)
    );
    return topCall();
}

void TapeMachine::popCall() {
    callStack_.pop_back();
}

bool TapeMachine::step() {
    auto call = topCall();
    if (call->next()) {
        switch (call->getOperation()) {
            case NativeTape::Nope:
                break;
            case NativeTape::GetConst:
                call->getConst();
                break;
            case NativeTape::GetLocal:
                call->getLocal();
                break;
            case NativeTape::SetLocal:
                call->setLocal();
                break;
            case NativeTape::RunFunction:
                runFunctionImpl(call);
                break;
            case NativeTape::Skip:
                call->skipUntilMark();
                break;
            case NativeTape::SkipIfNil:
                {
                    auto guard = call->pop();
                    if (guard.is<Nil>()) {
                        call->skipUntilMark();
                    }
                }
                break;
            case NativeTape::GuardMark:
                break;
            case NativeTape::LocalStackRewind:
                call->stackRewind();
                break;
            case NativeTape::RunTailRecOptimizedFunction:
                if (callStack_.size() < 2) {
                    runFunctionImpl(call);
                } else {
                    runTailRecOptimizedFunctionImpl(call);
                }
                break;
            case NativeTape::InvalidUpLimit:
                throw Error() << "Invalid native operation";
                break;
        }
    } else {
        // save return value
        ret_ = call->pop();
        // preserve the last local vars (for import)
        vars_ = call->releaseLocals();
        // drop spent call
        popCall();
        // should we resume something from the stack?
        if (this->isStackEmpty()) {
            // done, nothing to do
            return false;
        }
        // resume it
        call = this->topCall();
        call->push(
            std::move(ret_)
        );
        ret_ = Cell::nil();
    }
    return true;
}

}  // namespace AntLisp
