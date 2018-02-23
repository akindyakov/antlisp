#include "stack_machine.h"


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
    //**/std::cerr << "Create NativeFunctionCall, runner: " << runner << '\n';
}

NativeFunctionDefinition::EOperations NativeFunctionCall::getOperation() const {
    return function->operations[runner].operation;
}

bool NativeFunctionCall::next() noexcept {
    //**/std::cerr << "runner++: " << runner + 1 << '\n';
    return ++runner < function->operations.size();
}

Cell NativeFunctionCall::popCallStack() {
    return localCallStack.pop();
}

void NativeFunctionCall::pushCallStack(Cell cell) {
    //**/std::cerr << "stack push(" << localCallStack.size() << "): " << cell.toString() << '\n';
    localCallStack.push(
        std::move(cell)
    );
}

void NativeFunctionCall::getConst() {
    // copy
    auto cell = function->consts[
        function->operations[runner].position
    ];
    //**/std::cerr << "Get const: " << cell.toString() << " (" << runner << ")\n";
    this->pushCallStack(
        std::move(cell)
    );
}

void NativeFunctionCall::getLocal() {
    auto pos = this->function->operations[
        this->runner
    ].position;
    const auto& name = this->function->names[pos];
    //**/std::cerr << "Get: " << name << "  (" << runner << ")\n";
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
    this->vars[name] =  this->popCallStack();
}

void NativeFunctionCall::getGlobal(const Namespace& global) {
    const auto& name = function->names[
        function->operations[runner].position
    ];
    //**/std::cerr << "Get: " << name << "  (" << runner << ")\n";
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
    //**/std::cerr << "next\n";
    switch (call->getOperation()) {
        case Nope:
            //**/std::cerr << "nope\n";
            break;
        case GetConst:
            //**/std::cerr << "get const\n";
            call->getConst();
            break;
        case GetLocal:
            //**/std::cerr << "get local\n";
            call->getLocal();
            break;
        case SetLocal:
            //**/std::cerr << "set local\n";
            call->setLocal();
            break;
        case GetGlobal:
            //**/std::cerr << "get global\n";
            call->getGlobal(env.vars);
            break;
        case SetGlobal:
            call->setGlobal(env.vars);
            break;
        case RunFunction:
            {
                //**/std::cerr << "run function\n";
                auto args = call->createArgs();
                auto toCall = call->popCallStack();
                if (toCall.is<FunctionPtr>()) {
                    auto fnPtr = toCall.get<FunctionPtr>();
                    if (fnPtr->isNative()) {
                        //**/std::cerr << "run native function\n";
                        call->next();  // to get back to the right place on native return
                        call = env.stackPush(
                            fnPtr->nativeCall(args)
                        );
                        return true;
                    } else {
                        //**/std::cerr << "run instant function\n";
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
        //**/std::cerr << "no next\n";
        env.ret = std::move(
            call->popCallStack()
        );
        env.stackPop();
        if (env.isStackEmpty()) {
            //**/std::cerr << "stack is empty\n";
            return false;
        }
        if (env.ret.is<FunctionPtr>()) {
            auto fnPtr = env.ret.get<FunctionPtr>();
            if (fnPtr->isPostponed()) {
                call = env.stackPush(
                    fnPtr->nativeCall(Arguments{})
                );
                env.ret = Cell::nil();
                return true;
            }
        }
        call = env.stackTop();
        call->pushCallStack(
            std::move(env.ret)
        );
        env.ret = Cell::nil();
    }
    return true;
}

}  // namespace AntLisp
