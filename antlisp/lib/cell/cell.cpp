#include "cell.h"


namespace AntLisp {

template<>
Cell Cell::cast<Nil>() const {
    if (not this->is<Nil>()) {
        throw BadGetError()
            << "Wrong cast to Nil: "
            << this->toString()
        ;
    }
    return Cell::nil();
}

template<>
Cell Cell::cast<Integer>() const {
    auto out = Integer{};
    if (this->is<Integer>()) {
        out = this->as<Integer>();
    } else if (this->is<Float>()) {
        out = static_cast<Integer>(
            this->as<Float>()
        );
    } else if (this->is<Symbol>()) {
        out = static_cast<Integer>(
            this->as<Symbol>()
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
            this->as<Integer>()
        );
    } else if (this->is<Float>()) {
        out = this->as<Float>();
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
        out = this->as<Symbol>();
    } else if (this->is<Integer>()) {
        out = static_cast<Symbol>(
            this->as<Integer>()
        );
    } else {
        throw BadGetError()
            << "Wrong cast to Symbol: "
            << this->toString()
        ;
    }
    return Cell::symbol(out);
}

template<>
Cell Cell::cast<FunctionPtr>() const {
    if (not this->is<FunctionPtr>()) {
        throw BadGetError()
            << "Wrong cast to FunctionPtr: "
            << this->toString()
        ;
    }
    return this->copy();
}

Cell Cell::copy() const {
    return Cell{this->valuePtr_->copy()};
}

std::string Cell::toString() const {
    return this->valuePtr_->toString();
}

template<>
std::string CellType<FunctionPtr>::toString() const;
template<>
ICellType::Ptr CellType<FunctionPtr>::copy() const;

template<>
std::string CellType<Integer>::toString() const {
    return std::to_string(value_);
}

template<>
std::string CellType<Float>::toString() const {
    return std::to_string(value_);
}

template<>
std::string CellType<Symbol>::toString() const {
    return std::to_string(value_);
}

}  // namespace AntLisp
