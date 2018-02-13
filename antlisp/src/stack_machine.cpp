#include "antlisp/src/stack_machine.h"

namespace AntLisp {

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
        case GetGlobal:
            call->getGlobal(env.vars);
            break;
        case SetGlobal:
            call->setGlobal(env.vars);
            break;
        case RunFunction:
            {
                auto args = call->createArgs();
                auto fdef = call->popLocal().get<FunctionDefinitionPtr>();
                call = env.stackPush(
                    FunctionCall(
                        std::move(fdef),
                        std::move(args)
                    )
                );
            }
            break;
        case RunExternalFunction:
            {
                auto frame = call->createArgs();
                auto fdef = call->popLocal().get<ExtFunctionPtr>();
                fdef->call(frame);
                if (!frame.empty()) {
                    call->pushLocal(
                        std::move(frame.back())
                    );
                } else {
                    call->pushLocal(
                        Cell::nil()
                    );
                }
            }
            break;
        case StackRewind:
            call->stackRewind();
            break;
        case SkipIfTrue:
            {
                auto guard = call->popLocal();
                if (guard.is<Nil>()) {
                    call->skip();
                }
            }
            break;
        case SkipIfFalse:
            {
                auto guard = call->popLocal();
                if (!guard.is<Nil>()) {
                    call->skip();
                }
            }
            break;
    }
    if (!call->next()) {
        env.ret = std::move(
            call->popLocal()
        );
        env.stackPop();
        if (env.isStackEmpty()) {
            return false;
        }
        call = env.stackTop();
        call->pushLocal(
            std::move(env.ret)
        );
    }
    return true;
}

}  // namespace AntLisp
