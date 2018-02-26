#pragma once

#include <antlisp/src/cell/cell.h>

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
        GetConst,
        GetLocal,
        SetLocal,
        RunFunction,
        StackRewind,
        Skip,
        SkipIfNil,
        GuardMark,
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
        : public StackMachineError
    {
    };

    static bool step(Environment& env);

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

    bool next() noexcept;

    Cell popCallStack();

    void pushCallStack(Cell cell);

    void getConst();

    void getLocal();

    void setLocal();

    void getGlobal(const Namespace& global);

    void setGlobal(Namespace& global);

    void stackRewind();

    void skipUntilMark();

    Arguments createArgs();

    Namespace releaseLocals() {
        auto locals = std::move(vars);
        vars.clear();
        return locals;
    }

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

    virtual Cell instantCall(
        Arguments frame
    ) const = 0;

    virtual NativeFunctionCall nativeCall(
        Arguments args
    ) const = 0;

    static Namespace parseArguments(
        Arguments& args
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
    explicit NativeFunction()
        : fdef(
            std::make_shared<NativeFunctionDefinition>()
        )
    {
    }

    explicit NativeFunction(
        NativeFunctionDefinitionPtr fdef_
        , std::size_t argnum_
        , Namespace closures_
    )
        : argnum(argnum_)
        , fdef(
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
    ) const {
        vars.insert(closures.begin(), closures.end());
        return std::make_shared<NativeFunction>(fdef, argnum, std::move(vars));
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
            args,
            fdef->names,
            argnum
        );
        vars.insert(closures.begin(), closures.end());
        return NativeFunctionCall(fdef, vars);
    }

    bool hasName(
        const TVarName& name
    ) const {
        auto last = fdef->names.cbegin() + argnum;
        //**/ std::cerr << "native arg names (\n";
        //**/ for (auto it = fdef->names.cbegin(); it != last; ++it) {
        //**/     std::cerr << " " << *it;
        //**/ }
        //**/ std::cerr << ")\n";
        return (
            closures.count(name) != 0
            || last != std::find(fdef->names.cbegin(), last, name)
        );
    }

public:
    std::size_t argnum = 0;
    NativeFunctionDefinitionPtr fdef;
    Namespace closures;  // default arguments
};

class LambdaFunction
    : public IFunction
{
public:
    explicit LambdaFunction(
        NativeFunction native_
        , ArgNames names_
    )
        : nativeFn(std::move(native_))
        , names(std::move(names_))
    {
        //**/ std::cerr << "create lambda with arg names (\n";
        //**/ for (const auto& name : names) {
        //**/     std::cerr << " " << name;
        //**/ }
        //**/ std::cerr << ")\n";
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

    Cell instantCall(
        Arguments args
    ) const override {
        auto nextClosures = IFunction::parseArguments(
            args,
            names,
            names.size()
        );
        return Cell::function(
            nativeFn.activate(nextClosures)
        );
    }

    NativeFunctionCall nativeCall(
        Arguments
    ) const override final {
        throw Error() << "Method 'nativeCall' is not valid for 'LambdaFunction'";
        return NativeFunctionCall{
            nullptr,
            Namespace{}
        };
    }

    NativeFunctionDefinition* core() {
        return nativeFn.fdef.get();
    }

    bool hasName(
        const TVarName& name
    ) const {
        //**/ std::cerr << "lambda arg names (\n";
        //**/ for (const auto& name : names) {
        //**/     std::cerr << " " << name;
        //**/ }
        //**/ std::cerr << ")\n";
        return (
            nativeFn.hasName(name)
            || names.end() != std::find(names.begin(), names.end(), name)
        );
    }

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
    )
    {
        CallStack.push_back(
            std::move(toRun)
        );
    }
    explicit Environment(
        const NativeFunction& toRun
    )
        : Environment(
            toRun.nativeCall(
                Arguments{}
            )
        )
    {
    }

    bool step();

    void run() {
        while (this->step()) {
        }
    }

    class Error
        : public StackMachineError
    {
    };

private:
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

public:
    Namespace vars;
    Cell ret;

private:
    std::vector<NativeFunctionCall> CallStack;
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
