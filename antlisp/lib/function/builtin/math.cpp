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
    } else if (to.is<StringPtr>() && arg.is<StringPtr>()) {
        to.get<StringPtr>()->append(
            *arg.get<StringPtr>()
        );
    } else if (to.is<StringPtr>() && arg.is<Symbol>()) {
        to.get<StringPtr>()->push_back(
            arg.get<Symbol>()
        );
    } else if (to.is<Symbol>() && arg.is<Symbol>()) {
        auto str = std::string(2, to.get<Symbol>());
        str[1] = arg.get<Symbol>();
        to = Cell::string(
            std::move(str)
        );
    } else if (to.is<Symbol>() && arg.is<StringPtr>()) {
        auto str = std::string(1, to.get<Symbol>());
        str.append(
            *arg.get<StringPtr>()
        );
        to = Cell::string(std::move(str));
    } else {
        throw RuntimeError() << "Sum: unexpected argument types ( "
            << to.toString() << ", " <<  arg.toString() << " )";
    }
}

}

Cell Sum::instantCall(
    Arguments args
) const {
    auto out = Cell::nil();
    auto it = args.cbegin();
    if (it != args.cend()) {
        out = it->copy();
        while (++it != args.cend()) {
            sumImpl(out, *it);
        }
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
    } else if (left.is<StringPtr>() && right.is<StringPtr>()) {
        if (
            *left.get<StringPtr>() < *right.get<StringPtr>()
        ) {
            return true;
        }
    } else if (left.is<Symbol>() && right.is<Symbol>()) {
        if (
            left.get<Symbol>() < right.get<Symbol>()
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
    // TODO: make it possible to use more than two args
    if (args.size() != 2) {
        throw TypeError() << "Less function takes exactly 2 arguments";
    }
    auto out = lessImpl(args.front(), args.back())
        ? Cell::t()
        : Cell::nil();
    return out;
}

Cell Equality::instantCall(
    Arguments args
) const {
    auto out = Cell::t();
    auto it = args.begin();
    auto first = it++;
    while (it < args.end()) {
        if (*it != *first) {
            out = Cell::nil();
            break;
        }
        ++it;
    }
    return out;
}

void allMathFunctions(Namespace& space) {
    space.emplace("+", std::make_shared<Sum>());
    space.emplace("<", std::make_shared<Less>());
    space.emplace("=", std::make_shared<Equality>());
}

}  // namespace Builtin

}  // namespace AntLisp

