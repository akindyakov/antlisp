#include "math.h"

#include <antlisp/lib/util/exception.h>

#include <antlisp/lib/cell_ext/string/string.h>


namespace AntLisp {

namespace Builtin {

namespace {

void sumImpl(Cell& to, const Cell& arg) {
    if (to.is<Integer>() && arg.is<Integer>()) {
        to.as<Integer>() += arg.as<Integer>();
    } else if (to.is<Float>() || arg.is<Float>()) {
        to = to.cast<Float>();
        to.as<Float>() += arg.cast<Float>().as<Float>();
    } else {
        throw RuntimeError() << "Sum: unexpected argument types ( "
            << to.toString() << ", " <<  arg.toString() << " )";
    }
}

}

Cell Sum::instantCall(
    Arguments args
) const {
    if (args.size() < 2) {
        throw TypeError() << "Sum function takes 2 or more arguments";
    }
    auto out = Cell::nil();
    auto it = args.cbegin();
    out = it->copy();
    while (++it != args.cend()) {
        sumImpl(out, *it);
    }
    return out;
}

namespace {

void multiplicationImpl(Cell& to, const Cell& right) {
    if (to.is<Integer>() && right.is<Integer>()) {
        to.as<Integer>() *= right.as<Integer>();
    } else if (to.is<Float>() || right.is<Float>()) {
        if (not to.is<Float>()) {
            to = to.cast<Float>();
        }
        to.as<Float>() *= right.cast<Float>().as<Float>();
    } else {
        throw RuntimeError() << "Multiplication: unexpected argument types ( "
            << to.toString() << ", " <<  right.toString() << " )";
    }
}

}

Cell Multiplication::instantCall(
    Arguments args
) const {
    if (args.size() < 2) {
        throw TypeError() << "Multiplication function takes 2 or more arguments";
    }
    auto out = Cell::nil();
    auto it = args.cbegin();
    out = it->copy();
    while (++it != args.cend()) {
        multiplicationImpl(out, *it);
    }
    return out;
}

namespace {

void divisionImpl(
    Cell& what
    , const Cell& right
) {
    if (what.is<Integer>() && right.is<Integer>()) {
        what.as<Integer>() /= right.as<Integer>();
    } else if (what.is<Float>() || right.is<Float>()) {
        if (not what.is<Float>()) {
            what = what.cast<Float>();
        }
        what.as<Float>() /= right.cast<Float>().as<Float>();
    } else {
        throw RuntimeError()
            << "Division: unexpected argument types ( "
            << what.toString() << ", " <<  right.toString() << " )"
        ;
    }
}

}

Cell Division::instantCall(
    Arguments args
) const {
    if (args.size() < 2) {
        throw TypeError() << "Division function takes 2 or more arguments";
    }
    auto out = Cell::nil();
    auto it = args.cbegin();
    out = it->copy();
    while (++it != args.cend()) {
        divisionImpl(out, *it);
    }
    return out;
}

namespace {

bool lessImpl(const Cell& left, const Cell& right) {
    if (left.is<Integer>() && right.is<Integer>()) {
        if (left.as<Integer>() < right.as<Integer>()) {
            return true;
        }
    } else if (left.is<Float>() || right.is<Float>()) {
        if (
            left.cast<Float>().as<Float>()
            < right.cast<Float>().as<Float>()
        ) {
            return true;
        }
    } else if (left.is<Symbol>() && right.is<Symbol>()) {
        if (
            left.as<Symbol>() < right.as<Symbol>()
        ) {
            return true;
        }
    } else {
        throw RuntimeError() << "Less: unexpected argument types ( "
            << left.toString() << ", " <<  right.toString() << " )";
    }
    return false;
}

}

Cell Less::instantCall(
    Arguments args
) const {
    if (args.size() < 2) {
        throw TypeError() << "Less function takes 2 or more arguments";
    }
    auto right = args.cbegin();
    auto left = right++;
    while (right != args.cend()) {
        if (not lessImpl(*left, *right)) {
            return Cell::nil();
        }
        ++left;
        ++right;
    }
    return Cell::t();
}
namespace {

bool eqImpl(const Cell& left, const Cell& right) {
    const auto id = left.typeId();
    if (id != right.typeId()) {
        return false;
    }
    if (id == Cell::typeIdOf<Nil>()) {
        return true;
    } else if (id == Cell::typeIdOf<Integer>()) {
        return left.as<Integer>() == right.as<Integer>();
    } else if (id == Cell::typeIdOf<Float>()) {
        return left.as<Float>() == right.as<Float>();
    } else if (id == Cell::typeIdOf<Symbol>()) {
        return left.as<Symbol>() == right.as<Symbol>();
    } else if (id == Cell::typeIdOf<StringType>()) {
        return left.as<StringType>() == right.as<StringType>();
    }
    return false;
}

}

Cell Equality::instantCall(
    Arguments args
) const {
    auto right = args.cbegin();
    auto left = right++;
    while (right != args.cend()) {
        if (not eqImpl(*left, *right)) {
            return Cell::nil();
        }
        ++left;
        ++right;
    }
    return Cell::t();
}

void allMathFunctions(Namespace& space) {
    space.emplace("+", Cell::function(std::make_shared<Sum>()));
    space.emplace("*", Cell::function(std::make_shared<Multiplication>()));
    space.emplace("<", Cell::function(std::make_shared<Less>()));
    space.emplace("=", Cell::function(std::make_shared<Equality>()));
    space.emplace("/", Cell::function(std::make_shared<Division>()));
}

}  // namespace Builtin

}  // namespace AntLisp

