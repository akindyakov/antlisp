#pragma once

#include <antlisp/src/cell/cell.h>

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

class StackMachineError
    : public Exception
{
};

class LocalStack
{
public:
    void push(Cell value) {
        stackImpl.push_back(
            std::move(value)
        );
    }

    Cell pop() {
        auto value = std::move(
            stackImpl.back()
        );
        stackImpl.pop_back();
        return value;
    }

    void skip(std::size_t count) {
        if (stackImpl.size() < count) {
            throw StackMachineError() << __FILE__ << ":" << __LINE__;
        }
        stackImpl.resize(
            stackImpl.size() - count
        );
    }

    std::size_t size() const noexcept {
        return stackImpl.size();
    }

private:
    std::vector<Cell> stackImpl;
};

struct Environment;

struct NativeFunctionDefinition {
    NativeFunctionDefinition() = default;

    NativeFunctionDefinition(const NativeFunctionDefinition&) = delete;
    NativeFunctionDefinition(NativeFunctionDefinition&&) = default;

    NativeFunctionDefinition& operator=(const NativeFunctionDefinition&) = delete;
    NativeFunctionDefinition& operator=(NativeFunctionDefinition&&) = default;

    virtual ~NativeFunctionDefinition() = default;
    enum EOperations {
        Nope,
        GetConst,
        GetLocal,
        SetLocal,
        GetGlobal,
        SetGlobal,
        RunFunction,
        StackRewind,
        SkipIfTrue,
        SkipIfFalse,
    };

    struct Step {
        explicit Step(
            EOperations op
            , std::size_t pos
        )
            : operation(op)
            , position(pos)
        {
        }
        EOperations operation;
        std::size_t position = 0;
    };

    std::vector<Step> operations;
    std::size_t argnum;
    std::vector<TVarName> names;
    std::vector<Cell> consts;  // unnamed

    void getGlobalName(
        const TVarName& name
    );

    class Error
        : public StackMachineError
    {
    };

    static bool step(Environment& env);
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

    bool next() noexcept;

    Cell popCallStack();

    void pushCallStack(Cell cell);

    void getConst();

    void getLocal();

    void setLocal();

    void getGlobal(const Namespace& global);

    void setGlobal(Namespace& global);

    void stackRewind();

    void skip();

    Arguments createArgs();

private:
    NativeFunctionDefinitionPtr function;
    LocalStack localCallStack;
    Namespace vars;
    std::size_t runner = 0;
};

class IFunction {
public:
    ~IFunction() = default;

    virtual bool isPostponed() const = 0;
    virtual bool isNative() const = 0;

    virtual FunctionPtr activate(
        Namespace vars
    ) const = 0;

    virtual Cell instantCall(
        Arguments frame
    ) const = 0;

    virtual NativeFunctionCall nativeCall(
        Arguments args
    ) const = 0;

    static Namespace parseArguments(
        Arguments args
        , const std::vector<TVarName>& names
        , const std::size_t argMaxNum
    ) {
        auto vars = Namespace{};
        std::size_t nameIndex = 0;
        auto rit = args.rbegin();
        while (rit != args.rend() && nameIndex < argMaxNum) {
            const auto& argName = names[nameIndex];
            vars[argName] = std::move(*rit);
            ++rit;
            ++nameIndex;
        }
        return vars;
    }
};

class ExtInstantFunction
    : public IFunction
{
public:
    class Error
        : public StackMachineError
    {
    };

    bool isPostponed() const final {
        return false;
    }

    bool isNative() const override final {
        return false;
    }

    FunctionPtr activate(
        Namespace vars
    ) const override {
        throw Error() << "Using 'activate' method for 'ExtInstantFunction' is not valid";
        return nullptr;
    }

    virtual Cell instantCall(
        Arguments frame
    ) const = 0;

    NativeFunctionCall nativeCall(
        Arguments frame
    ) const override final {
        throw Error() << "Method 'nativeCall' is not valid for 'ExtInstantFunction'";
        return NativeFunctionCall(nullptr, Namespace{});
    }
};

class ExtSum
    : public ExtInstantFunction
{
    Cell instantCall(
        Arguments frame
    ) const override {
        auto sum = Integer{0};
        for (const auto& cell : frame) {
            sum += cell.get<Integer>();
        }
        return Cell::integer(sum);
    }
};

class ExtMultiplication
    : public ExtInstantFunction
{
    Cell instantCall(
        Arguments frame
    ) const override {
        auto m = Integer{1};
        for (const auto& cell : frame) {
            m *= cell.get<Integer>();
        }
        return Cell::integer(m);
    }
};

class NativeFunction
    : public IFunction
{
public:
    explicit NativeFunction(
        NativeFunctionDefinitionPtr fdef_
        , Namespace closures_
    )
        : fdef(
            std::move(fdef_)
        )
        , closures(
            std::move(closures_)
        )
    {
    }

    NativeFunction(const NativeFunction&) = delete;
    NativeFunction(NativeFunction&&) = default;

    NativeFunction& operator=(const NativeFunction&) = delete;
    NativeFunction& operator=(NativeFunction&&) = default;

    class Error
        : public StackMachineError
    {
    };

    bool isPostponed() const override {
        return false;
    }

    bool isNative() const override final {
        return true;
    }

    FunctionPtr activate(
        Namespace vars
    ) const override {
        vars.insert(closures.begin(), closures.end());
        return std::make_shared<NativeFunction>(fdef, std::move(vars));
    }

    Cell instantCall(
        Arguments args
    ) const override {
        throw Error() << "Method 'nativeCall' is not valid for 'NativeFunction'";
        return Cell::nil();
    }

    NativeFunctionCall nativeCall(
        Arguments args
    ) const override {
        auto vars = IFunction::parseArguments(
            std::move(args),
            fdef->names,
            fdef->argnum
        );
        vars.insert(closures.begin(), closures.end());
        return NativeFunctionCall(fdef, vars);
    }

private:
    NativeFunctionDefinitionPtr fdef;
    Namespace closures;
};

struct LambdaFunctionDefinition {
    explicit LambdaFunctionDefinition(
        FunctionPtr nextFunctionPtr_
        , std::vector<TVarName> names_
    )
        : nextPtr(
            std::move(nextFunctionPtr_)
        )
        , names(
            std::move(names_)
        )
    {
    }

    FunctionPtr nextPtr;
    std::vector<TVarName> names;
};

using LambdaFunctionDefinitionPtr = std::shared_ptr<LambdaFunctionDefinition>;

class LambdaFunction
    : public IFunction
{
public:
    explicit LambdaFunction(
        FunctionPtr functionPtr
        , std::vector<TVarName> names_
        , Namespace closures_
    )
        : def(
            std::make_shared<LambdaFunctionDefinition>(
                std::move(functionPtr),
                std::move(names_)
            )
        )
        , closures(std::move(closures_))
    {
    }

    explicit LambdaFunction(
        LambdaFunctionDefinitionPtr def_
        , Namespace closures_
    )
        : def(std::move(def_))
        , closures(std::move(closures_))
    {
    }

    class Error
        : public StackMachineError
    {
    };

    bool isPostponed() const override {
        return false;
    }

    bool isNative() const override {
        return false;
    }

    FunctionPtr activate(
        Namespace vars
    ) const override {
        vars.insert(closures.begin(), closures.end());
        return std::make_shared<LambdaFunction>(
            def,
            std::move(vars)
        );
    }

    Cell instantCall(
        Arguments args
    ) const override {
        auto nextClosures = IFunction::parseArguments(
            std::move(args),
            def->names,
            def->names.size()
        );
        nextClosures.insert(closures.begin(), closures.end());
        return Cell::function(
            def->nextPtr->activate(nextClosures)
        );
    }

    NativeFunctionCall nativeCall(
        Arguments frame
    ) const override final {
        throw Error() << "Method 'nativeCall' is not valid for 'LambdaFunction'";
        return NativeFunctionCall{
            nullptr,
            Namespace{}
        };
    }

private:
    LambdaFunctionDefinitionPtr def;
    Namespace closures;
};

struct Environment {
    std::vector<NativeFunctionCall> CallStack;
    Namespace vars;
    Cell ret;

    bool isStackEmpty() {
        return this->CallStack.empty();
    }

    NativeFunctionCall* stackTop() {
        return &this->CallStack.back();
    }

    NativeFunctionCall* stackPush(
        NativeFunctionCall frame
    ) {
        this->CallStack.push_back(
            std::move(frame)
        );
        return this->stackTop();
    }

    void stackPop() {
        return this->CallStack.pop_back();
    }
};



/**
* Как быть с заполнением let ?
* GetFunction
* ...GetLocalVar...
* ...GetGlobalVar...
* RunFunction - результат run нужно положить в LocalScope наверх, как в стек
* когда выйдем за скобку let - stackRewind на количество переменных
*/

/**
* Что с возвращаемым значением?
* Мы его кладем в stack самым последним - так ведь?
* Выйдя из функции мы будем иметь заполненый LocalScope - в вызывающей функции можем взять столько сколько нужно аргументов.
*
* Где записать количество возвращаемых аргументов?
* position для RunFunction будет означать сколько переменных с конца LocalScope забрать себе.
* Вызываемая функция может вызывать сколько угодно значений, только вызывающая определяет сколько оттуда взять. Но всегда с конца.
* Но как она это поймет?
* Нее, пусть возвращается только последниее значение.
*/

/**
* Все не POD типы передаются по указателю. Может быть в том числе строки. ХЗ пока.
* Для передачи в качестве аргумента копии объекта необходимы специальные функции (copy x) и (deep-copy x)
*/

}  // namespace AntLisp
