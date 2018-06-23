#pragma once

#include <antlisp/lib/cell/cell_type.h>

#include <string>
#include <vector>

namespace AntLisp {

class LispError {
public:
    using CodeType = std::uint32_t;
    static constexpr CodeType defaultCode = 0u;

public:
    explicit LispError() = default;
    explicit LispError(
        std::string msg
    );

    explicit LispError(
        CodeType errCode
        , std::string msg
    );

    static LispError inherit(
        const LispError& err
    );

    static LispError inherit(
        const LispError& err
        , CodeType errCode
        , std::string msg
    );

    std::string message() const;

private:
    std::vector<CodeType> err_code_;
    std::string message_;
};

using LispErrorCell = CellType<LispError>;

template<>
std::string CellType<LispError>::toString() const;

template<>
ICellType::Ptr CellType<LispError>::copy() const;

}  // namespace AntLisp
