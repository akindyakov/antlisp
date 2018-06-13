#include "native_tape.h"

namespace AntLisp {

void NativeTape::ApplyTailRecursionOptimization() {
    if (operations.empty()) {
        return;
    }
    if (operations.back().operation == RunFunction) {
        operations.back().operation = RunTailRecOptimizedFunction;
    } else if (operations.back().operation == GuardMark) {
        auto guard = operations.back().operand;
        for (
            auto rit = operations.rbegin();
            rit != operations.rend();
            ++rit
        ) {
            if (
                (rit->operation == Skip or rit->operation == SkipIfNil)
                and rit->operand == guard
            ) {
                auto nextOpIt = std::next(rit);
                if (nextOpIt->operation == RunFunction) {
                    nextOpIt->operation = RunTailRecOptimizedFunction;
                }
            }
        }
    }
}

}  // namespace AntLisp
