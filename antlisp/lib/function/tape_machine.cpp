#include "tape_machine.h"

#include <antlisp/lib/util/string.h>

#include <algorithm>


namespace AntLisp {

template<>
std::string CellType<FunctionPtr>::toString() const {
    return value_->toString();
}

void LocalStack::push(Cell value) {
    stackImpl.push_back(
        std::move(value)
    );
}

Cell LocalStack::pop() {
    auto value = std::move(
        stackImpl.back()
    );
    stackImpl.pop_back();
    return value;
}

std::size_t LocalStack::size() const noexcept {
    return stackImpl.size();
}

void NativeTape::ApplyTailRecursionOptimization() {
    if (operations.empty()) {
        return;
    }
    if (operations.back().operation == RunFunction) {
        operations.back().operation = RunTailRecOptimizedFunction;
    } else if (operations.back().operation == GuardMark) {
        auto guard = operations.back().position;
        for (
            auto rit = operations.rbegin();
            rit != operations.rend();
            ++rit
        ) {
            if (
                (rit->operation == Skip or rit->operation == SkipIfNil)
                and rit->position == guard
            ) {
                auto nextOpIt = std::next(rit);
                if (nextOpIt->operation == RunFunction) {
                    nextOpIt->operation = RunTailRecOptimizedFunction;
                }
            }
        }
    }
}

Namespace IFunction::parseArguments(
    Arguments& args
    , const std::vector<TVarName>& names
    , const std::size_t argMaxNum
) {
    auto vars = Namespace{};
    std::size_t nameIndex = 0;
    auto rit = args.begin();
    while (rit != args.end() && nameIndex < argMaxNum) {
        const auto& argName = names[nameIndex];
        vars[argName] = std::move(*rit);
        ++rit;
        ++nameIndex;
    }
    return vars;
}

NativeFunction::NativeFunction(
    NativeFunctionDefinitionPtr fdef_
    , std::size_t argnum_
    , Namespace closures_
    , TVarName selfName_
)
    : argnum(argnum_)
    , fdef(
        std::move(fdef_)
    )
    , closures(
        std::move(closures_)
    )
    , selfName(
        std::move(selfName_)
    )
{
    closures[selfName] = Cell::function(
        nullptr
    );
}

bool NativeFunction::isNative() const {
    return true;
}

FunctionPtr NativeFunction::activate(
    Namespace vars
) const {
    for (const auto& var : closures) {
        vars.emplace(var.first, var.second.copy());
    }
    auto copy = std::make_shared<NativeFunction>(
        fdef, argnum, std::move(vars), selfName
    );
    // Add this ptr to local vars to make recursive call
    copy->closures[selfName] = Cell::function(
        copy
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
    for (const auto& var : closures) {
        vars.emplace(var.first, var.second.copy());
    }
    return NativeFunctionCall(fdef, std::move(vars));
}

bool NativeFunction::hasName(
    const TVarName& name
) const {
    auto last = fdef->names.cend();
    return (
        closures.count(name) != 0
        || last != std::find(fdef->names.cbegin(), last, name)
    );
}

std::string NativeFunction::toString() const {
    std::ostringstream out;
    {
        out << "( ";
        auto last = fdef->names.cbegin() + argnum;
        for (auto it = fdef->names.cbegin(); it != last; ++it) {
            out << *it << " ";
        }
        out << ")\n";
    }
    for (const auto& closure : closures) {
        if (closure.second.is<FunctionPtr>()) {
            out << "( " << closure.first << " <lambda> )\n";
        } else {
            out << "( " << closure.first
                << ' ' << closure.second.toString()
                << '\n'
            ;
        }
    }
    for (const auto& op : fdef->operations) {
        out << int(op.operation) << " : " << int(op.position) << "\n";
    }
    return out.str();
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
    const TVarName& name
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
    : function(
        std::move(fdef)
    )
    , vars(
        std::move(predefinedVars)
    )
{
    --runner;
}

NativeTape::EOperations NativeFunctionCall::getOperation() const {
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
    auto cell = function->consts[
        function->operations[runner].position
    ].copy();
    this->push(
        std::move(cell)
    );
}

void NativeFunctionCall::getLocal() {
    auto pos = this->function->operations[
        this->runner
    ].position;
    const auto& name = this->function->names[pos];
    auto localIt = this->vars.find(name);
    if (localIt == this->vars.end()) {
        throw RuntimeError() << "There is no such local name " << Str::Quotes(name);
    }
    this->push(localIt->second.copy());
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
            step.operation == NativeTape::GuardMark
            && step.position == mark
        ) {
            break;
        }
    }
}

Arguments NativeFunctionCall::createArgs() {
    auto size = function->operations[runner].position;
    auto args = Arguments(size);
    for (auto& arg : args) {
        arg = pop();
    }
    std::reverse(args.begin(), args.end());
    return args;
}

Namespace NativeFunctionCall::releaseLocals() {
    auto locals = std::move(vars);
    vars.clear();
    return locals;
}

TapeMachine::TapeMachine(
    NativeFunctionCall toRun
)
{
    CallStack.push_back(
        std::move(toRun)
    );
}

TapeMachine::TapeMachine(
    const NativeFunction& toRun
)
    : TapeMachine(
        toRun.nativeCall(
            Arguments{}
        )
    )
{
}

void TapeMachine::runFunctionImpl(NativeFunctionCall* call) {
    auto args = call->createArgs();
    auto cellToRun = call->pop();
    this->runCellWithArguments(call, cellToRun, std::move(args));
}

void TapeMachine::runTailRecOptimizedFunctionImpl(
    NativeFunctionCall* call
) {
    auto args = call->createArgs();
    auto cellToRun = call->pop();
    this->popCall();
    this->runCellWithArguments(this->topCall(), cellToRun, std::move(args));
}

void TapeMachine::runCellWithArguments(
    NativeFunctionCall* call
    , Cell& cellToRun
    , Arguments args
) {
    if (cellToRun.is<FunctionPtr>()) {
        auto fnPtr = cellToRun.as<FunctionPtr>();
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

bool TapeMachine::isStackEmpty() const {
    return this->CallStack.empty();
}

NativeFunctionCall* TapeMachine::topCall() {
    return &this->CallStack.back();
}

NativeFunctionCall* TapeMachine::pushCall(
    NativeFunctionCall frame
) {
    this->CallStack.push_back(
        std::move(frame)
    );
    return this->topCall();
}

void TapeMachine::popCall() {
    this->CallStack.pop_back();
}

bool TapeMachine::step() {
    auto call = this->topCall();
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
                this->runFunctionImpl(call);
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
