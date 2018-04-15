#include "string.h"

#include <antlisp/lib/util/string.h>


namespace AntLisp {

std::string StringType::toString() const {
    return Str::Quotes(value);
}

ExtTypePtr StringType::copy() const {
    return std::make_shared<StringType>(
        value
    );
}

void StringType::summarize(const Cell& arg) {
    if (arg.is<StringPtr>()) {
        value.append(
            *arg.get<StringPtr>()
        );
    } else if (arg.is<Symbol>()) {
        value.push_back(
            arg.get<Symbol>()
        );
    } else {
        throw RuntimeError()
            << "Summarize to string: unexpected argument type ( "
            <<  arg.toString() << " )";
    }
}

void StringType::multiplication(const Cell& arg) {
    if (arg.is<Integer>()) {
        auto len = value.size();
        auto n = arg.get<Integer>();
        if (n < 0) {
            throw RuntimeError() << "Multiplication: multiplication string by negative integer is prohibited";
        }
        while (n-- > 1) {
            std::copy(
                value.begin(),
                value.begin() + len,
                std::back_inserter(*strPtr)
            );
        }
    } else {
        throw RuntimeError()
            << "Multiply string by: unexpected argument type ( "
            <<  arg.toString() << " )";
    }
}

}  // namespace AntLisp
