#pragma once

#include <string>
#include <memory>


namespace AntLisp {

using TypeId = std::size_t;

class ICellType {
public:

    virtual ~ICellType() = default;
    using Ptr = std::unique_ptr<ICellType>;

    virtual TypeId typeId() const noexcept = 0;
    virtual std::string toString() const = 0;
    virtual Ptr copy() const = 0;
};

class Nil {
public:
    constexpr Nil() noexcept = default;
};

constexpr inline bool operator == (Nil, Nil) noexcept {
    return true;
}

std::ostream& operator<<(std::ostream& os, const Nil& v);

template<
    typename T
>
class CellType
    : public ICellType
{
public:
    // TODO(akindyakov): think about another type of CellType with intrusive_ptr incide for large external objects
    using Type = T;
    using SelfType = CellType<Type>;
    using ConstType = typename std::add_const<Type>::type;

    static const TypeId typeId_;

public:
    template<
        typename... Args
    >
    explicit CellType(Args&&... args)
        : value_(std::forward<Args>(args)...)
    {
    }

    virtual ~CellType() = default;

    TypeId typeId() const noexcept override {
        return typeId_;
    }
    T take() {
        return std::move(value_);
    }
    T& at() noexcept {
        return value_;
    }
    const T& cAt() const noexcept {
        return value_;
    }
    const T& at() const noexcept {
        return cAt();
    }

    static constexpr const char* name() {
        // TODO(akindyakov): use boost::core::typeinfo to generate informative CellType name
        return typeid(Type).name();
    }

    std::string toString() const override {
        // TODO(akindyakov): make a real implementation of generic CellType<T>::toString()
        auto ret = std::string{"instance of "};
        ret += '"';
        ret += name();
        ret += '"';
        return ret;
    }

    ICellType::Ptr copy() const override {
        return std::make_unique<SelfType>(value_);
    }

private:
    T value_;
};

template<
    typename LeftArgType
    , typename RightArgType
    , typename = typename std::enable_if<
        std::is_same<LeftArgType, RightArgType>::value
    >::type
>
bool operator==(
    const CellType<LeftArgType>& left
    , const CellType<RightArgType>& right
) {
    return left.typeId() == right.typeId() && left.at() == right.at();
}

TypeId genTypeId() noexcept;

template<
    typename Type
>
const TypeId CellType<Type>::typeId_ = genTypeId();

}  // namespace AntLisp
