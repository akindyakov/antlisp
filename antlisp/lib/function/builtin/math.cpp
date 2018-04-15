#include "math.h"

#include <antlisp/lib/util/exception.h>


namespace AntLisp {

namespace Builtin {

namespace {

void sumImpl(Cell& to, const Cell& arg) {
    if (to.is<Integer>() && arg.is<Integer>()) {
        to.get<Integer>() += arg.get<Integer>();
    } else if (to.is<Float>() || arg.is<Float>()) {
        to = to.cast<Float>();
        to.get<Float>() += arg.cast<Float>().get<Float>();
    } else if (to.is<ExtTypePtr>()) {
        to.get<ExtTypePtr>()->summarize(arg);
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
        to.get<Integer>() *= right.get<Integer>();
    } else if (to.is<Float>() || right.is<Float>()) {
        if (not to.is<Float>()) {
            to = to.cast<Float>();
        }
        to.get<Float>() *= right.cast<Float>().get<Float>();
    } else if (to.is<ExtTypePtr>()) {
        to.get<ExtTypePtr>()->multiply(right);
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
        what.get<Integer>() /= right.get<Integer>();
    } else if (what.is<Float>() || right.is<Float>()) {
        if (not what.is<Float>()) {
            what = what.cast<Float>();
        }
        what.get<Float>() /= right.cast<Float>().get<Float>();
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
        if (left.get<Integer>() < right.get<Integer>()) {
            return true;
        }
    } else if (left.is<Float>() || right.is<Float>()) {
        if (
            left.cast<Float>().get<Float>()
            < right.cast<Float>().get<Float>()
        ) {
            return true;
        }
    } else if (left.is<Symbol>() && right.is<Symbol>()) {
        if (
            left.get<Symbol>() < right.get<Symbol>()
        ) {
            return true;
        }
    } else if (left.is<ExtTypePtr>()) {
        return left.get<ExtTypePtr>()->less(right);
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

Cell Equality::instantCall(
    Arguments args
) const {
    auto right = args.cbegin();
    auto left = right++;
    while (right != args.cend()) {
        if (*left != *right) {
            return Cell::nil();
        }
        ++left;
        ++right;
    }
    return Cell::t();
}

void allMathFunctions(Namespace& space) {
    space.emplace("+", std::make_shared<Sum>());
    space.emplace("*", std::make_shared<Multiplication>());
    space.emplace("<", std::make_shared<Less>());
    space.emplace("=", std::make_shared<Equality>());
    space.emplace("/", std::make_shared<Division>());
}

}  // namespace Builtin

}  // namespace AntLisp

