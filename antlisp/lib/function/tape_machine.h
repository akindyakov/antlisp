#pragma once

#include "native_tape.h"
#include "type_vocabulary.h"

#include <antlisp/lib/cell/cell.h>

#include <algorithm>
#include <exception>
#include <iostream>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>


namespace AntLisp {

class LocalStack
{
public:
    void push(Cell value);

    Cell pop();

    std::size_t size() const noexcept;

private:
    std::vector<Cell> cells_;
};

class ExtInstantFunction;
using InstantFunctionPtr = std::shared_ptr<ExtInstantFunction>;

class NativeFunctionCall {
public:
    explicit NativeFunctionCall(
        NativeFunctionDefinitionPtr fdef
        , Namespace predefinedVars
    );

    NativeTape::EOperations getOperation() const;

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
    std::size_t runner_ = 0;
    NativeFunctionDefinitionPtr function_;
    LocalStack localCallStack_;
    Namespace vars_;
};

class IFunction {
public:
    virtual ~IFunction() = default;

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

template<>
std::string CellType<FunctionPtr>::toString() const;

class ExtInstantFunction
    : public IFunction
{
public:
    virtual ~ExtInstantFunction() = default;

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
        // TODO(akindyakov): please implement toString methdod for ExtInstantFunction
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
    virtual ~NativeFunction() = default;

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

    virtual ~LambdaFunction() = default;

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

    NativeTape* core();

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

class TapeMachine {
public:
    explicit TapeMachine(
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

    const Cell& ret() const {
        return ret_;
    }

    Namespace takeVars() {
      return std::move(vars_);
    }

    const Namespace& vars() const {
      return vars_;
    }

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

private:
    Namespace vars_;
    Cell ret_;
    std::vector<NativeFunctionCall> callStack_;
};

}  // namespace AntLisp
