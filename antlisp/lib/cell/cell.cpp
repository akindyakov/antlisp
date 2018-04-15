#include "cell.h"


namespace AntLisp {

std::string MockExtType::toString() const override {
    return "ext-type";
}

ExtTypePtr MockExtType::copy() const override {
    throw NotImplementedError() << "\"copy\" method does not implemented";
}
void MockExtType::summarize(const Cell&) override {
    throw NotImplementedError() << "\"summarize\" method does not implemented";
}
void MockExtType::multiply(const Cell&) override {
    throw NotImplementedError() << "\"multiply\" method does not implemented";
}
void MockExtType::subtract(const Cell&) override {
    throw NotImplementedError() << "\"subtract\" method does not implemented";
}
void MockExtType::divide(const Cell&) override {
    throw NotImplementedError() << "\"divide\" method does not implemented";
}

template<>
Cell Cell::cast<Nil>() const {
    if (not this->is<Nil>()) {
        throw BadGetError()
            << "Wrong cast to Nil: "
            << this->toString()
        ;
    }
    return *this;
}

template<>
Cell Cell::cast<Integer>() const {
    auto out = Integer{};
    if (this->is<Integer>()) {
        out = this->get<Integer>();
    } else if (this->is<Float>()) {
        out = static_cast<Integer>(
            this->get<Float>()
        );
    } else if (this->is<Symbol>()) {
        out = static_cast<Integer>(
            this->get<Symbol>()
        );
    } else {
        throw BadGetError()
            << "Wrong cast to Integer: "
            << this->toString()
        ;
    }
    return Cell::integer(out);
}

template<>
Cell Cell::cast<Float>() const {
    auto out = Float{};
    if (this->is<Integer>()) {
        out = static_cast<Float>(
            this->get<Integer>()
        );
    } else if (this->is<Float>()) {
        out = this->get<Float>();
    } else {
        throw BadGetError()
            << "Wrong cast to Float: "
            << this->toString()
        ;
    }
    return Cell::real(out);
}

template<>
Cell Cell::cast<Symbol>() const {
    auto out = Symbol{};
    if (this->is<Symbol>()) {
        out = this->get<Symbol>();
    } else if (this->is<Integer>()) {
        out = static_cast<Symbol>(
            this->get<Integer>()
        );
    } else {
        throw BadGetError()
            << "Wrong cast to Symbol: "
            << this->toString()
        ;
    }
    return Cell{out};
}

template<>
Cell Cell::cast<StringPtr>() const {
    // shell we use the simlest one ?
    // return Cell::string(
    //     this->toString()
    // );
    if (not this->is<StringPtr>()) {
        throw BadGetError()
            << "Wrong cast to StringPtr: "
            << this->toString()
        ;
    }
    return *this;
}

template<>
Cell Cell::cast<ExtTypePtr>() const {
    // TODO(akindyakov): [task](doc/todo/5t7dmp19.md)
    if (not this->is<ExtTypePtr>()) {
        throw BadGetError()
            << "Wrong cast to ExtTypePtr: "
            << this->toString()
        ;
    }
    return *this;
}

template<>
Cell Cell::cast<FunctionPtr>() const {
    if (not this->is<FunctionPtr>()) {
        throw BadGetError()
            << "Wrong cast to FunctionPtr: "
            << this->toString()
        ;
    }
    return *this;
}

Cell Cell::copy() const {
    if (this->is<StringPtr>()) {
        return Cell::string(
            *this->get<StringPtr>()
        );
    } else if (this->is<ExtTypePtr>()) {
        return Cell{
            this->get<ExtTypePtr>()->copy()
        };
    }
    return *this;
}

namespace {

class ToStringVisitor
{
public:
    using result_type = std::string;

public:
    template<typename T>
    std::string operator()(const T& v) const {
        return TypeInfo<T>::toString(v);
    }
};

}

std::string Cell::toString() const {
    return this->visit(
        ToStringVisitor()
    );
}

namespace {

class CellEqualityVisitor
{
private:
    const Cell& first;

public:
    using result_type = bool;

public:
    explicit CellEqualityVisitor(
        const Cell& first_
    )
        : first(first_)
    {
    }

    template<typename T>
    bool operator()(const T& value) const {
        return (
            Cell::tagOf<T>() == first.tag()
            && value == first.get<T>()
        );
    }

    bool operator()(const StringPtr& value) const {
        return (
            Cell::tagOf<StringPtr>() == first.tag()
            && *value == *first.get<StringPtr>()
        );
    }
};

}

bool operator == (
    const Cell& first
    , const Cell& second
) {
    return first.visit(
        CellEqualityVisitor(second)
    );
}

std::ostream& operator<<(std::ostream& os, const Nil& v) {
    os << TypeInfo<Nil>::toString(v);
    return os;
}

bool operator != (
    const Cell& first
    , const Cell& second
) {
    return !(first == second);
}

}  // namespace AntLisp
