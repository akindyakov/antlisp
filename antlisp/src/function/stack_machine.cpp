#include "stack_machine.h"

#include <antlisp/src/util/string.h>


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
    //**/ std::cerr << "Create NativeFunctionCall, runner: " << runner << '\n';
}

NativeFunctionDefinition::EOperations NativeFunctionCall::getOperation() const {
    return function->operations[runner].operation;
}

bool NativeFunctionCall::next() noexcept {
    //**/ std::cerr << "runner++: " << runner + 1 << '\n';
    return ++runner < function->operations.size();
}

Cell NativeFunctionCall::popCallStack() {
    return localCallStack.pop();
}

void NativeFunctionCall::pushCallStack(Cell cell) {
    //**/ std::cerr << "stack push(" << localCallStack.size() << "): " << cell.toString() << '\n';
    localCallStack.push(
        std::move(cell)
    );
}

void NativeFunctionCall::getConst() {
    // copy
    auto cell = function->consts[
        function->operations[runner].position
    ];
    //**/ std::cerr << "Get const: " << cell.toString() << " (" << runner << ")\n";
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
    //**/ std::cerr << "Get local: " << Str::Quotes(name)
    //**/ << " = " << local.toString()
    //**/ << " (" << runner << ")\n";
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

void NativeFunctionCall::stackRewind() {
    localCallStack.skip(
        function->operations[runner].position
    );
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

bool Environment::step() {
    auto call = this->stackTop();
    //**/ std::cerr << "next\n";
    switch (call->getOperation()) {
        case NativeFunctionDefinition::Nope:
            //**/ std::cerr << "nope\n";
            break;
        case NativeFunctionDefinition::GetConst:
            //**/ std::cerr << "get const\n";
            call->getConst();
            break;
        case NativeFunctionDefinition::GetLocal:
            //**/ std::cerr << "get local\n";
            call->getLocal();
            break;
        case NativeFunctionDefinition::SetLocal:
            //**/std::cerr << "set local\n";
            call->setLocal();
            break;
        case NativeFunctionDefinition::RunFunction:
            {
                //**/ std::cerr << "run function\n";
                auto args = call->createArgs();
                auto toCall = call->popCallStack();
                if (toCall.is<FunctionPtr>()) {
                    auto fnPtr = toCall.get<FunctionPtr>();
                    if (fnPtr->isNative()) {
                        //**/ std::cerr << "run native function\n";
                        call->next();  // to get back to the right place on native return
                        call = this->stackPush(
                            fnPtr->nativeCall(args)
                        );
                        return true;
                    } else {
                        //**/ std::cerr << "run instant function\n";
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
            //**/ std::cerr << "Skip\n";
            call->skipUntilMark();
            break;
        case NativeFunctionDefinition::SkipIfNil:
            {
                //**/ std::cerr << "SkipIfNil\n";
                auto guard = call->popCallStack();
                if (guard.is<Nil>()) {
                    //**/ std::cerr << "skip ->\n";
                    call->skipUntilMark();
                }
            }
            break;
        case NativeFunctionDefinition::GuardMark:
            //**/ std::cerr << "GuardMark\n";
            break;
        case NativeFunctionDefinition::LocalStackRewind:
            call->stackRewind();
            break;
    }
    if (!call->next()) {
        //**/ std::cerr << "no next\n";
        this->ret = std::move(
            call->popCallStack()
        );
        this->vars = std::move(
            call->releaseLocals()
        );
        this->stackPop();
        if (this->isStackEmpty()) {
            //**/ std::cerr << "stack is empty\n";
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
