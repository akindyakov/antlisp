#pragma once

#include "type_vocabulary.h"

#include <antlisp/lib/cell/cell.h>

#include <memory>
#include <vector>
#include <ostream>


namespace AntLisp {

struct NativeTape {
    explicit NativeTape() = default;

    explicit NativeTape(
        ArgNames names_
    )
        : names(std::move(names_))
    {
    }

    NativeTape(const NativeTape&) = delete;
    NativeTape(NativeTape&&) = default;

    NativeTape& operator=(const NativeTape&) = delete;
    NativeTape& operator=(NativeTape&&) = default;

    virtual ~NativeTape() = default;

    enum EOperations
        : int
    {
        Nope = 0,
        GetConst,       // 1
        GetLocal,       // 2
        SetLocal,       // 3
        RunFunction,    // 4
        Skip,           // 5
        SkipIfNil,      // 6
        GuardMark,      // 7
        LocalStackRewind,  // 8
        RunTailRecOptimizedFunction,  // 9
        InvalidUpLimit,
    };

    struct Step {
        using Operand = std::size_t;
        explicit Step(
            EOperations op
            , std::size_t pos
        )
            : operation(op)
            , operand(pos)
        {
        }
        EOperations operation;
        Operand operand = 0;
    };

    void addStep(
        EOperations op
        , std::size_t pos
    ) {
        operations.emplace_back(op, pos);
    }

    class Error
        : public TapeMachineError
    {
    };

    void ApplyTailRecursionOptimization();

    std::string toString() const;

    std::vector<Step> operations;
    // TODO(akindyakov): use unique values in NativeTape::names
    std::vector<VarName> names;
    std::vector<Cell> consts;  // unnamed
};

using NativeFunctionDefinitionPtr = std::shared_ptr<NativeTape>;

const std::string& toString(NativeTape::EOperations op);

std::ostream& operator<<(std::ostream& os, NativeTape::EOperations op);

}  // namespace AntLisp
