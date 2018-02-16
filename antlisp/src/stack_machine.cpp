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
                auto toCall = call->popLocal();
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
                    call->pushLocal(
                        extf->call(
                            std::move(args)
                        )
                    );
                } else {
                    throw Error() << __FILE__ << ":" << __LINE__;
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
        if (env.ret.is<PostponedFunctionPtr>()) {
            call = env.stackPush(
                FunctionCall(
                    std::move(
                        env.ret.get<PostponedFunctionPtr>()
                    )
                )
            );
            call = env.stackTop();
        } else {
            call = env.stackTop();
            call->pushLocal(
                std::move(env.ret)
            );
        }
        env.ret = Cell::nil();
    }
    return true;
}

}  // namespace AntLisp
