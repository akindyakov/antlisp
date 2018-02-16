#pragma once

#include "exception.h"

#include <boost/variant.hpp>
#include <boost/variant/get.hpp>

#include <string>
#include <unordered_map>


namespace AntLisp {

class FunctionDefinition;
class PostponedFunction;
class IExtFunction;

using FunctionDefinitionPtr = std::shared_ptr<FunctionDefinition>;
using PostponedFunctionPtr  = std::shared_ptr<FunctionDefinition>;
using ExtFunctionPtr        = std::shared_ptr<IExtFunction>;

class Nil
{
public:
    constexpr Nil() noexcept = default;
};

constexpr inline bool operator == (Nil, Nil) noexcept {
    return true;
}

std::ostream& operator<<(std::ostream& os, const Nil& v);

class Cons;
using ConsPtr = std::shared_ptr<Cons>;

using StringPtr = std::shared_ptr<std::string>;

using Integer = long;
using Float = double;
using Symbol = std::string::value_type;

template<typename T>
class TypeInfo;

class Cell final
{
public:
    using Value = boost::variant<
        Nil
        , Integer
        , Float
        , Symbol
        , StringPtr
        , ConsPtr
        , FunctionDefinitionPtr
        , PostponedFunctionPtr
        , ExtFunctionPtr
    >;

    enum class Tag {
        Nil,
        Integer,
        Float,
        Symbol,
        StringPtr,
        Cons,
        FunctionDefinitionPtr,
        PostponedFunctionPtr,
        ExtFunctionPtr,
    };

    ~Cell() = default;

    class Error
        : public Exception
    {
    };

    explicit Cell()
        : value(Nil{})
    {
    }
    static Cell nil() {
        return Cell(Nil{});
    }

    template<
        typename SomeNumberType
    >
    static Cell integer(SomeNumberType&& v) {
        return Cell{
            static_cast<Integer>(
                std::forward<SomeNumberType>(v)
            )
        };
    }

    template<
        typename SomeStringType
    >
    static Cell string(SomeStringType&& v) {
        return Cell{
            std::make_shared<std::string>(
                std::string{
                    std::forward<SomeStringType>(v)
                }
            )
        };
    }

    template<
        typename SomeNumberType
    >
    static Cell real(SomeNumberType&& v) {
        return Cell{
            static_cast<Float>(
                std::forward<SomeNumberType>(v)
            )
        };
    }

    template<
        typename... Args
    >
    static Cell function(Args&&... args) {
        return Cell{
            std::make_shared<FunctionDefinition>(
                std::forward<Args>(args)...
            )
        };
    }

    template<
        typename ExtFunctionType
        , typename... Args
    >
    static Cell extFunction(Args&&... args) {
        return Cell{
            static_cast<ExtFunctionPtr>(
                std::make_shared<ExtFunctionType>(
                    std::forward<Args>(args)...
                )
            )
        };
    }

    explicit Cell(Nil v)
        : value(v)
    {
    }
    explicit Cell(Integer v)
        : value(v)
    {
    }
    explicit Cell(Float v)
        : value(v)
    {
    }
    explicit Cell(Symbol v)
        : value(v)
    {
    }
    explicit Cell(StringPtr v)
        : value(
            std::move(v)
        )
    {
    }
    explicit Cell(ConsPtr v)
        : value(
            std::move(v)
        )
    {
    }
    explicit Cell(FunctionDefinitionPtr ptr)
        : value(
            std::move(ptr)
        )
    {
    }
    explicit Cell(ExtFunctionPtr ptr)
        : value(
            std::move(ptr)
        )
    {
    }

    Cell(Cell&&) = default;
    Cell(const Cell&) = default;

    Cell& operator=(Cell&&) = default;
    Cell& operator=(const Cell&) = default;

    Tag tag() const {
        return static_cast<Tag>(
            value.which()
        );
    }

    template<typename T>
    static Tag tagOf() {
        return static_cast<Tag>(
            Value(T{}).which()
        );
    }

    template<typename T>
    bool is() const {
        return this->tag() == tagOf<T>();
    }

    class BadGetError
        : public Exception
    {
    };

    Float asRealNumber() const {
        auto realV = Float{};
        if (this->is<Float>()) {
            realV = this->get<Float>();
        } else if (this->is<Integer>()) {
            realV = static_cast<Float>(
                this->get<Integer>()
            );
        } else {
            throw BadGetError()
                << this->toString() << "is not a real number"
            ;
        }
        return realV;
    }

    template<typename T>
    const T& get() const {
        try {
            return boost::get<T>(value);
        } catch (const boost::bad_get& err) {
            throw BadGetError()
                << "Wrong type. "
                << this->toString()
                << " is not a " << TypeInfo<T>::name()
            ;
        }
    }

    template<typename T>
    T& get() {
        try {
            return boost::get<T>(value);
        } catch (const boost::bad_get& err) {
            throw BadGetError()
                << "Wrong type. "
                << this->toString()
                << " is not a " << TypeInfo<T>::name()
            ;
        }
    }

    template<typename T>
    using Visitor = boost::static_visitor<T>;

    template<
        typename TVisitor
        , typename TResult = typename TVisitor::result_type
    >
    TResult visit(TVisitor visitor) {
        return boost::apply_visitor(visitor, value);
    }

    template<
        typename TVisitor
        , typename TResult = typename TVisitor::result_type
    >
    TResult visit(const TVisitor& visitor) const {
        return boost::apply_visitor(visitor, value);
    }

    std::string toString() const;

public:
    Value value;
};

bool operator == (
    const Cell& first
    , const Cell& second
);

bool operator != (
    const Cell& first
    , const Cell& second
);

class Cons {
public:
    Cell car;
    Cell cdr;
};

const auto True = Cell{Integer{1}};

class ParserError
    : public Exception
{
};

bool tryFromString(
    Cell& dst
    , const std::string& str
);

}  // namespace AntLisp

#include "cell_impl.h"
