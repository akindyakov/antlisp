#include "native_tape.h"

#include <antlisp/lib/util/exception.h>

#include <unordered_map>

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

std::string NativeTape::toString() const {
    std::ostringstream out;
    out << "names: ( \n";
    auto enumerate = std::size_t{0};
    for (const auto & name : this->names) {
        out << "  " << enumerate++ << ": " << name << '\n';
    }
    out << ")\n";
    enumerate = 0;
    out << "consts: ( \n";
    for (const auto & value : this->consts) {
        out << "  " << enumerate++ << ": " << value.toString() << '\n';
    }
    out << ")\n";
    out << "steps: ( \n";
    for (const auto& op : this->operations) {
        out << "  "
            << op.operation << " (" << static_cast<int>(op.operation)
            << "): " << op.operand << "\n"
        ;
    }
    out << ")\n";
    return out.str();
}

const std::string& toString(NativeTape::EOperations op) {
    static const auto map = std::unordered_map<
        NativeTape::EOperations,
        std::string
    >{
        {NativeTape::Nope, "nope"},
        {NativeTape::GetConst, "get const"},
        {NativeTape::GetLocal, "get local"},
        {NativeTape::SetLocal, "set local"},
        {NativeTape::RunFunction, "run function"},
        {NativeTape::Skip, "skip"},
        {NativeTape::SkipIfNil, "skip if nil"},
        {NativeTape::GuardMark, "guard"},
        {NativeTape::LocalStackRewind, "pop from stack"},
        {NativeTape::RunTailRecOptimizedFunction, "run tail function"},
    };
    auto it = map.find(op);
    if (it == map.end()) {
        throw TypeError()
            << "There is no literal representation for operation number "
            << static_cast<int>(op)
        ;
    }
    return it->second;
}

std::ostream& operator<<(std::ostream& os, NativeTape::EOperations op) {
    os << toString(op);
    return os;
}

}  // namespace AntLisp
