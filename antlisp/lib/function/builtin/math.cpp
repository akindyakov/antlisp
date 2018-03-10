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
        throw RuntimeError() << "Unexpected argument types ( "
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
        std::cerr << "0 " << out.toString() << "\n";
        while (++it != args.cend()) {
            std::cerr << "1 " << it->toString() << "\n";
            sumImpl(out, *it);
        }
    }
    return out;
}

}  // namespace Builtin

}  // namespace AntLisp

