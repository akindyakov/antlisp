#include "cell_in_string.h"

#include "error.h"


namespace AntLisp {

boost::optional<Cell> tryCellFromString(
    const std::string& str
) {
    auto out = boost::optional<Cell>{};
    if (str.empty()) {
        return out;
    }
    if (CellParser<Nil>::check(str)) {
        out = CellParser<Nil>::parse(str);

    } else if (CellParser<Float>::check(str)) {
        out = CellParser<Float>::parse(str);

    } else if (CellParser<StringPtr>::check(str)) {
        out = CellParser<StringPtr>::parse(str);

    } else if (CellParser<Symbol>::check(str)) {
        out = CellParser<Symbol>::parse(str);

    }
    return out;
}

bool CellParser<Nil>::check(const std::string& str) {
    return (
        str == nilName
        || str == trueName
    );
}

Cell CellParser<Nil>::parse(const std::string& str) {
    return (
        str == nilName
        ? Cell::nil()
        : Cell::t()
    );
}

bool CellParser<Float>::check(const std::string& str) {
    auto ch = str.front();
    return std::isdigit(ch) || ch == minusMark || ch == decimalMark;
}

Cell CellParser<Float>::parse(const std::string& str) {
    size_t pos = str.find(decimalMark);
    if (pos == std::string::npos) {
        pos = str.find(divMark);
        if (pos == std::string::npos) {
            return parseInteger(str);
        } else {
            return parseRational(str, pos);
        }
    }
    return parseFloat(str);
}

Cell CellParser<Float>::parseInteger(const std::string& str) {
    return Cell::integer(
        std::atoll(str.c_str())
    );
}

Cell CellParser<Float>::parseFloat(const std::string& str) {
    auto end = (char*){};
    return Cell::real(
        std::strtod(str.c_str(), &end)
    );
}

Cell CellParser<Float>::parseRational(
    const std::string& str
    , size_t slashPos
) {
    auto numerator = std::atoll(str.substr(0, slashPos).c_str());
    auto deNumerator = std::atoll(str.substr(slashPos + 1).c_str());
    return Cell::real(
        static_cast<Float>(
            numerator
        )
        / static_cast<Float>(
            deNumerator
        )
    );
}

bool CellParser<Symbol>::check(
    const std::string& str
) {
    return str.front() == '#';
}

Cell CellParser<Symbol>::parse(
    const std::string& str
) {
    auto ch = Symbol{};
    if (str.size() == 3) {
        ch = str.back();
    } else {
        throw ParseError() << "Simple character input should have prefix '#\\'";
    }
    return Cell(ch);
}

bool CellParser<StringPtr>::check(const std::string& str) {
    return str.front() == quote;
}

Cell CellParser<StringPtr>::parse(const std::string& str) {
    auto value = std::string(
        str.begin() + 1,
        str.end() - 1
    );
    return Cell{
        std::make_shared<StringType>(
            std::string(
                str.begin() + 1,
                str.end() - 1
            )
        )
    };
}

}  // namespace AntLisp
