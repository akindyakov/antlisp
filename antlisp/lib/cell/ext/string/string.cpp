#include "string.h"

#include <antlisp/lib/util/string.h>


namespace AntLisp {

std::string StringType::toString() const {
    std::cerr << "StringType::toString()\n";
    return Str::Quotes(value);
}

ExtTypePtr StringType::copy() const {
    return std::make_shared<StringType>(
        value
    );
}

void StringType::summarize(const Cell& arg) {
    if (
        auto strPtr = StringType::castFromCell(arg)
    ) {
        value.append(strPtr->value);
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

void StringType::multiply(const Cell& arg) {
    if (arg.is<Integer>()) {
        auto len = value.size();
        auto n = arg.get<Integer>();
        if (n < 0) {
            throw RuntimeError() << "Multiplication: multiplication string by negative integer is prohibited";
        }
        auto fromIt = value.begin();
        auto toIt = value.begin() + len;
        while (n-- > 1) {
            std::copy(
                fromIt,
                toIt,
                std::back_inserter(value)
            );
        }
    } else {
        throw RuntimeError()
            << "Multiply string by: unexpected argument type ( "
            <<  arg.toString() << " )";
    }
}

bool StringType::equal(const Cell& other) const {
    if (
        auto strPtr = StringType::castFromCell(other)
    ) {
        return value == strPtr->value;
    }
    throw RuntimeError()
        << "Summarize to string: unexpected argument type ( "
        <<  other.toString() << " )";
    return false;
}

bool StringType::less(const Cell& other) const {
    if (
        auto strPtr = StringType::castFromCell(other)
    ) {
        return value < strPtr->value;
    }
    throw RuntimeError()
        << "Summarize to string: unexpected argument type ( "
        <<  other.toString() << " )";
    return false;
}

}  // namespace AntLisp
