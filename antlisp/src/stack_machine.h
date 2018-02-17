#pragma once

#include "cell.h"

#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <exception>
#include <memory>


namespace AntLisp {

using TVarName = std::string;

using LocalStack = std::vector<Cell>;
using Namespace = std::unordered_map<TVarName, Cell>;

class IExtFunction {
public:
    virtual Cell call(
        LocalStack frame
    ) const = 0;
};

class ExtSum
    : public IExtFunction
{
    Cell call(
        LocalStack frame
    ) const override {
        auto sum = Integer{0};
        for (const auto& cell : frame) {
            sum += cell.get<Integer>();
        }
        return Cell::integer(sum);
    }
};

class ExtPrint
    : public IExtFunction
{
    Cell call(
        LocalStack frame
    ) const override {
        auto m = Integer{1};
        for (const auto& cell : frame) {
            m *= cell.get<Integer>();
        }
        return Cell::integer(m);
    }
};

struct Environment;

struct FunctionDefinition {
    virtual ~FunctionDefinition() = default;
    enum EOperations {
        Nope,
        GetConst,
        GetLocal,
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

    class Error
        : public Exception
    {
    };

    static constexpr auto InvalidPosition = std::numeric_limits<std::size_t>::max();

    std::vector<Step> operations;
    std::size_t argnum;
    std::vector<TVarName> argNames;
    std::vector<TVarName> names;
    LocalStack consts;

    void getGlobalName(
        const TVarName& name
    ) {
        auto pos = names.size();
        names.push_back(name);
        operations.emplace_back(
            FunctionDefinition::GetGlobal,
            pos
        );
    }

    static bool step(Environment& env);
};

struct PostponedFunction
{
    FunctionDefinitionPtr fdef;
    LocalStack args;
};

class FunctionCall {
public:
    explicit FunctionCall(
        FunctionDefinitionPtr fdef
        // positional args
        // named args - use tuple please for it
        , LocalStack args
    )
        : function(
            std::move(fdef)
        )
        , localCallStack(
            std::move(args)
        )
    {
        for (const auto& argName : this->function->argNames) {
            vars.emplace(argName, this->popCallStack());
        }
    }

    FunctionDefinition::EOperations getOperation() const {
        return function->operations[runner].operation;
    }

    bool next() noexcept {
        return ++runner < function->operations.size();
    }

    Cell popCallStack() {
        auto value = std::move(
            localCallStack.back()
        );
        localCallStack.pop_back();
        return value;
    }

    void pushCallStack(Cell cell) {
        localCallStack.push_back(
            std::move(cell)
        );
    }

    void getConst() {
        // copy
        auto cell = function->consts[
            function->operations[runner].position
        ];
        this->pushCallStack(
            std::move(cell)
        );
    }

    void getLocal() {
        // copy
        auto local = vars.at(
            function->names[
                function->operations[runner].position
            ]
        );
        this->pushCallStack(
            std::move(local)
        );
    }

    void getGlobal(const Namespace& global) {
        const auto& name = function->names[
            function->operations[runner].position
        ];
        // copy
        this->pushCallStack(
            global.at(name)
        );
    }

    void setGlobal(Namespace& global) {
        const auto& name = function->names[
            function->operations[runner].position
        ];
        global[name] = this->popCallStack();
    }

    void stackRewind() {
        localCallStack.resize(
            localCallStack.size() - function->operations[runner].position
        );
    }

    void skip() {
        runner += function->operations[runner].position;
    }

    LocalStack createArgs() {
        auto size = function->operations[runner].position;
        auto newFrame = LocalStack{};
        while (size--) {
            newFrame.push_back(
                popCallStack()
            );
        }
        return newFrame;
    }

private:
    FunctionDefinitionPtr function;
    LocalStack localCallStack;
    Namespace vars;
    std::size_t runner = 0;
};

struct Environment {
    std::vector<FunctionCall> CallStack;
    Namespace vars;
    Cell ret;

    bool isStackEmpty() {
        return this->CallStack.empty();
    }

    FunctionCall* stackTop() {
        return &this->CallStack.back();
    }

    FunctionCall* stackPush(
        FunctionCall frame
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
