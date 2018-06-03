#pragma once

#include "cell_type.h"

#include <antlisp/lib/util/exception.h>

#include <boost/variant.hpp>
#include <boost/variant/get.hpp>
#include <boost/optional.hpp>

#include <string>
#include <unordered_map>
#include <iostream>


namespace AntLisp {

class IFunction;

using FunctionPtr = std::shared_ptr<IFunction>;
using FunctionCell = CellType<FunctionPtr>;

using Integer = long;
using Float = double;
using Symbol = std::string::value_type;

class Cell final
{
public:
    Cell(Cell&&) = default;
    Cell& operator=(Cell&&) = default;

    Cell(const Cell&) = delete;
    Cell& operator=(const Cell&) = delete;

    ~Cell() = default;

    class Error
        : public Exception
    {
    };

    // TODO: remove default constructor for Cell
    explicit Cell()
        : valuePtr_(
            std::make_unique<CellType<Nil>>()
        )
    {
    }

    explicit Cell(
        std::unique_ptr<ICellType> ptr
    )
        : valuePtr_(std::move(ptr))
    {
    }

    static Cell nil() {
        return Cell{};
    }

    static Cell t() {
        return Cell::integer(1);
    }

    template<
        typename SomeNumberType
    >
    static Cell integer(SomeNumberType&& v) {
        return Cell{
            std::make_unique<CellType<Integer>>(
                static_cast<Integer>(
                    std::forward<SomeNumberType>(v)
                )
            )
        };
    }

    static Cell symbol(Symbol ch) {
        return Cell{
            std::make_unique<CellType<Symbol>>(ch)
        };
    }

    template<
      typename SomeNumberType
    >
    static Cell real(SomeNumberType&& v) {
        return Cell{
            std::make_unique<CellType<Float>>(
                static_cast<Float>(
                    std::forward<SomeNumberType>(v)
                )
            )
        };
    }

    static Cell function(FunctionPtr ptr) {
        return Cell{
            std::make_unique<FunctionCell>(
                ptr
            )
        };
    }

    template<
      typename Type
    >
    static Cell ext(Type&& value) {
        return Cell{
            std::make_unique<CellType<Type>>(
                value
            )
        };
    }

    TypeId typeId() const noexcept {
        return valuePtr_ ? valuePtr_->typeId() : CellType<Nil>::typeId_;
    }

    template<
        typename Type
    >
    static constexpr TypeId typeIdOf() noexcept {
        return CellType<Type>::typeId_;
    }

    template<
        typename Type
    >
    bool is() const noexcept {
        return this->typeId() == Cell::typeIdOf<Type>();
    }

    class BadGetError
        : public Exception
    {
    };

    template<
        typename Type
    >
    Type& as() {
        if (not this->is<Type>()) {
            throw BadGetError()
                << "Wrong type. "
                << this->toString()
                << " is not a " << CellType<Type>::name()
            ;
        }
        return static_cast<CellType<Type>*>(
            valuePtr_.get()
        )->at();
    }

    template<
        typename Type
    >
    const Type& cAs() const {
        if (not this->is<Type>()) {
            throw BadGetError()
                << "Wrong type. "
                << this->toString()
                << " is not a " << CellType<Type>::name()
            ;
        }
        return static_cast<const CellType<Type>*>(
            valuePtr_.get()
        )->cAt();
    }

    template<
        typename Type
    >
    const Type& as() const {
        return cAs<Type>();
    }

    template<typename Type>
    Cell cast() const;

    Cell copy() const;

    std::string toString() const;

public:
    std::unique_ptr<ICellType> valuePtr_;
};

const auto True = Cell::t();

template<>
std::string CellType<Integer>::toString() const;

template<>
std::string CellType<Float>::toString() const;

template<>
std::string CellType<Symbol>::toString() const;

}  // namespace AntLisp

// #include "cell_impl.h"

