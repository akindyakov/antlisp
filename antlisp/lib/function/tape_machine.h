#pragma once

#include <antlisp/lib/cell/cell.h>

#include <algorithm>
#include <exception>
#include <iostream>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>


namespace AntLisp {

using TVarName = std::string;

using Namespace = std::unordered_map<TVarName, Cell>;

using Arguments = std::vector<Cell>;
using ArgNames = std::vector<TVarName>;

class TapeMachineError
    : public Exception
{
};

class LocalStack
{
public:
    void push(Cell value);

    Cell pop();

    std::size_t size() const noexcept;

private:
    std::vector<Cell> stackImpl;
};

class Environment;

struct NativeFunctionDefinition {
    explicit NativeFunctionDefinition() = default;

    explicit NativeFunctionDefinition(
        ArgNames names_
    )
        : names(std::move(names_))
    {
    }

    NativeFunctionDefinition(const NativeFunctionDefinition&) = delete;
    NativeFunctionDefinition(NativeFunctionDefinition&&) = default;

    NativeFunctionDefinition& operator=(const NativeFunctionDefinition&) = delete;
    NativeFunctionDefinition& operator=(NativeFunctionDefinition&&) = default;

    virtual ~NativeFunctionDefinition() = default;

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
        RunTailRecOptimizedFunction  // 9
    };

    struct Step {
        using Operand = std::size_t;
        explicit Step(
            EOperations op
            , std::size_t pos
        )
            : operation(op)
            , position(pos)
        {
        }
        EOperations operation;
        Operand position = 0;  // FIXME: rename it to operand
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

    std::vector<Step> operations;
    std::vector<TVarName> names;
    std::vector<Cell> consts;  // unnamed
};

using NativeFunctionDefinitionPtr = std::shared_ptr<NativeFunctionDefinition>;

class ExtInstantFunction;
using InstantFunctionPtr = std::shared_ptr<ExtInstantFunction>;

class NativeFunctionCall {
public:
    explicit NativeFunctionCall(
        NativeFunctionDefinitionPtr fdef
        , Namespace predefinedVars
    );

    NativeFunctionDefinition::EOperations getOperation() const;

    bool good() noexcept;

    bool next() noexcept;

    Cell pop();

    void push(Cell cell);

    void getConst();

    void getLocal();

    void setLocal();

    void stackRewind();

    void skipUntilMark();

    Arguments createArgs();

    Namespace releaseLocals();

private:
    NativeFunctionDefinitionPtr function;
    LocalStack localCallStack;
    Namespace vars;
    std::size_t runner = 0;
};

class IFunction {
public:
    ~IFunction() = default;

    virtual bool isNative() const = 0;

    virtual Cell instantCall(
        Arguments args
    ) const = 0;

    virtual NativeFunctionCall nativeCall(
        Arguments args
    ) const = 0;

    virtual std::string toString() const = 0;

    static Namespace parseArguments(
        Arguments& args
        , const std::vector<TVarName>& names
        , const std::size_t argMaxNum
    );
};

class ExtInstantFunction
    : public IFunction
{
public:
    class Error
        : public TapeMachineError
    {
    };

    bool isNative() const override final {
        return false;
    }

    virtual Cell instantCall(
        Arguments args
    ) const override = 0;

    NativeFunctionCall nativeCall(
        Arguments /*args*/
    ) const override final {
        throw Error() << "Method 'nativeCall' is not valid for 'ExtInstantFunction'";
        return NativeFunctionCall(nullptr, Namespace{});
    }

    std::string toString() const override {
        // TODO(akindyakov)
        return std::string{"ext instant function"};
    }
};

class NativeFunction
    : public IFunction
{
public:
    explicit NativeFunction(
        NativeFunctionDefinitionPtr fdef_
        , std::size_t argnum_
        , Namespace closures_
        , TVarName selfName_
    );

    NativeFunction(const NativeFunction&) = delete;
    NativeFunction(NativeFunction&&) = default;

    NativeFunction& operator=(const NativeFunction&) = delete;
    NativeFunction& operator=(NativeFunction&&) = default;

    class Error
        : public TapeMachineError
    {
    };

    bool isNative() const override final;

    FunctionPtr activate(
        Namespace vars
    ) const;

    Cell instantCall(
        Arguments args
    ) const override;

    NativeFunctionCall nativeCall(
        Arguments args
    ) const override;

    bool hasName(
        const TVarName& name
    ) const;

    std::string toString() const override;

public:
    std::size_t argnum = 0;
    NativeFunctionDefinitionPtr fdef;
    Namespace closures;  // default arguments
    TVarName selfName;
};

class LambdaFunction
    : public IFunction
{
public:
    explicit LambdaFunction(
        NativeFunction native_
        , ArgNames names_
    );

    class Error
        : public TapeMachineError
    {
    };

    bool isNative() const override;

    Cell instantCall(
        Arguments args
    ) const override;

    NativeFunctionCall nativeCall(
        Arguments
    ) const override final;

    NativeFunctionDefinition* core();

    bool hasName(
        const TVarName& name
    ) const;

    std::string toString() const override;

public:
    // members visible only for parsers
    NativeFunction nativeFn;
    std::vector<TVarName> names;
};

using LambdaFunctionPtr = std::shared_ptr<LambdaFunction>;

class Environment {
public:
    explicit Environment(
        NativeFunctionCall toRun
    );
    explicit Environment(
        const NativeFunction& toRun
    );

    bool step();

    void run() {
        while (this->step()) {
        }
    }

    class Error
        : public TapeMachineError
    {
    };

private:
    bool isStackEmpty() const;
    NativeFunctionCall* topCall();
    NativeFunctionCall* pushCall(
        NativeFunctionCall frame
    );
    void popCall();
    void runFunctionImpl(NativeFunctionCall* call);
    void runTailRecOptimizedFunctionImpl(
        NativeFunctionCall* call
    );
    void runCellWithArguments(
        NativeFunctionCall* call
        , Cell& cellToRun
        , Arguments args
    );

public:
    Namespace vars;
    Cell ret;

private:
    std::vector<NativeFunctionCall> CallStack;
};

}  // namespace AntLisp