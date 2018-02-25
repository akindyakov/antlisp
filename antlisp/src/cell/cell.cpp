#include "cell.h"


namespace AntLisp {

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

bool operator == (
    const Cell& first
    , const Cell& second
) {
    return first.value == second.value;
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

namespace {

class TrueValueParser {
public:
    static bool checkPrefix(const std::string& str) {
        static const auto literal = std::string{"true"};
        return str == literal;
    }
    static Cell parse(const std::string&) {
        return Cell::t();
    }
};

class NumberValueParser
{
public:
    static constexpr char decimalMark = '.';
    static constexpr char minusMark = '-';
    static constexpr char divMark = '/';

public:
    static bool checkPrefix(const std::string& str) {
        auto ch = str.front();
        return std::isdigit(ch) || ch == minusMark || ch == decimalMark;
    }

    static Cell parseInteger(const std::string& str) {
        return Cell::integer(
            std::atoll(str.c_str())
        );
    }

    static Cell parseFloat(const std::string& str) {
        auto end = (char*){};
        return Cell::real(
            std::strtod(str.c_str(), &end)
        );
    }

    static Cell parseRational(const std::string& str, size_t slashPos) {
        auto numerator = std::atoll(str.substr(0, slashPos).c_str());
        auto deNumerator = std::atoll(str.substr(slashPos + 1).c_str());
        // TODO: use truly rational type here
        return Cell::real(
            static_cast<Float>(
                numerator
            )
            / static_cast<Float>(
                deNumerator
            )
        );
    }

    static Cell parse(const std::string& str) {
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
};

class SimpleCharacterParser {
public:
    static bool checkPrefix(const std::string& str) {
        return str.front() == '#';
    }
    static Cell parse(const std::string& str) {
        auto ch = Symbol{};
        if (str.size() == 3) {
            ch = str.back();
        } else {
            throw ParserError() << "Simple character input should have prefix '#\\'";
        }
        return Cell(ch);
    }
};

class StringValueParser {
public:
    static bool checkPrefix(const std::string& str) {
        return str.front() == quote;
    }
    static Cell parse(const std::string& str) {
        auto value = std::string(
            str.begin() + 1,
            str.end() - 1
        );
        return Cell::string(
            std::string(
                str.begin() + 1,
                str.end() - 1
            )
        );
    }

private:
    static constexpr char quote = '"';
};

}  // namespace

boost::optional<Cell> tryFromString(
    const std::string& str
) {
    auto out = boost::optional<Cell>{};
    if (str.empty()) {
        return out;
    }
    if (TrueValueParser::checkPrefix(str)) {
        out = TrueValueParser::parse(str);
    } else if (NumberValueParser::checkPrefix(str)) {
        out = NumberValueParser::parse(str);
    } else if (StringValueParser::checkPrefix(str)) {
        out = StringValueParser::parse(str);
    } else if (SimpleCharacterParser::checkPrefix(str)) {
        out = SimpleCharacterParser::parse(str);
    }
    return out;
}

}  // namespace AntLisp
