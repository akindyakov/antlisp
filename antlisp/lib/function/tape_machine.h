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

    std::size_t size() const noexcept {
        return stackImpl.size();
    }

    void print(std::ostream& out) {
        for (const auto& c : stackImpl) {
            out << "( " << c.toString() << " )\n";
        }
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
        GetConst,       // 1
        GetLocal,       // 2
        SetLocal,       // 3
        RunFunction,    // 4
        Skip,           // 5
        SkipIfNil,      // 6
        GuardMark,      // 7
        LocalStackRewind,  // 8
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

    bool isReadyToPostpone() const {
        return this->runner + 1 == function->operations.size();
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
    ) {
        auto vars = Namespace{};
        std::size_t nameIndex = 0;
        auto rit = args.begin();
        while (rit != args.end() && nameIndex < argMaxNum) {
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
        Arguments args
    ) const = 0;

    NativeFunctionCall nativeCall(
        Arguments args
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
    )
        : argnum(argnum_)
        , fdef(
            std::move(fdef_)
        )
        , closures(
            std::move(closures_)
        )
        , selfName(
            std::move(selfName_)
        )
    {
        // shold shared_from_this be used here?
        closures[selfName] = Cell::function(
            nullptr
        );
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
        auto copy = std::make_shared<NativeFunction>(
            fdef, argnum, std::move(vars), selfName
        );
        // Add this ptr to local vars to make recursive call
        copy->closures[selfName] = Cell::function(
            copy
        );
        // Not activated native function does not have 'this' variable
        return copy;
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
        auto last = fdef->names.cend();
        return (
            closures.count(name) != 0
            || last != std::find(fdef->names.cbegin(), last, name)
        );
    }

    std::string toString() const override {
        std::ostringstream out;
        {
            out << "( ";
            auto last = fdef->names.cbegin() + argnum;
            for (auto it = fdef->names.cbegin(); it != last; ++it) {
                out << *it << " ";
            }
            out << ")\n";
        }
        for (const auto& closure : closures) {
            out << "{ " << closure.first << " " << closure.second.toString() << " }\n";
        }
        for (const auto& op : fdef->operations) {
            out << int(op.operation) << " : " << int(op.position) << "\n";
        }
        return out.str();
    }

public:
    std::size_t argnum = 0;
    NativeFunctionDefinitionPtr fdef;
    Namespace closures;  // default arguments
    TVarName selfName;
};

class PostponedFunction
    : public IFunction
{
public:
    explicit PostponedFunction(
        NativeFunctionCall call
    )
        : call_(
            std::move(call)
        )
    {
    }

    bool isPostponed() const override {
        return true;
    }

    bool isNative() const override {
        return true;
    }

    Cell instantCall(
        Arguments args
    ) const override {
        throw Exception()
            << "Method 'nativeCall' is not valid for 'PostponedFunction'";
        return Cell::nil();
    }

    NativeFunctionCall nativeCall(
        Arguments args
    ) const override {
        return std::move(call_);
    }

    std::string toString() const override {
        std::ostringstream out;
        out << "postponed-function\n";
        return out.str();
    }

private:
    NativeFunctionCall call_;
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
        return (
            nativeFn.hasName(name)
            || names.end() != std::find(names.begin(), names.end(), name)
        );
    }

    std::string toString() const override {
        std::ostringstream out;
        out << "lambda: ( ";
        for (const auto& name : names) {
            out << name << " ";
        }
        out << ")\n";
        out << nativeFn.toString();
        return out.str();
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
        : public StackMachineError
    {
    };

private:
    bool isStackEmpty() {
        return this->CallStack.empty();
    }

    NativeFunctionCall* topCall() {
        return &this->CallStack.back();
    }

    NativeFunctionCall* pushCall(
        NativeFunctionCall frame
    ) {
        this->CallStack.push_back(
            std::move(frame)
        );
        return this->topCall();
    }

    void popCall() {
        this->CallStack.pop_back();
    }

    void runFunctionImpl(NativeFunctionCall* call);

public:
    Namespace vars;
    Cell ret;

private:
    std::vector<NativeFunctionCall> CallStack;
};

}  // namespace AntLisp
