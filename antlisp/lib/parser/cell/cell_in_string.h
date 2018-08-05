#pragma once

#include <antlisp/lib/cell/cell.h>
#include <antlisp/lib/cell_ext/string/string.h>

#include <optional>


namespace AntLisp {

std::optional<Cell> tryCellFromString(
    const std::string& str
);

template<
    typename Type
>
class CellParser;

template<>
class CellParser<Nil>
{
public:
    static constexpr auto nilName = "nil";
    static constexpr auto trueName = "t";

    static bool check(const std::string& str);
    static Cell parse(const std::string& str);
};

template<>
class CellParser<Float>
{
public:
    static constexpr char decimalMark = '.';
    static constexpr char minusMark = '-';
    static constexpr char divMark = '/';

public:
    static bool check(const std::string& str);
    static Cell parse(const std::string& str);

private:
    static Cell parseInteger(const std::string& str);
    static Cell parseFloat(const std::string& str);
    static Cell parseRational(const std::string& str, size_t slashPos);
};

template<>
class CellParser<Symbol>
{
public:
    static bool check(const std::string& str);
    static Cell parse(const std::string& str);
};

template<>
class CellParser<StringCell>
{
public:
    static constexpr char quote = '"';

public:
    static bool check(const std::string& str);
    static Cell parse(const std::string& str);
};

}  // namespace AntLisp
