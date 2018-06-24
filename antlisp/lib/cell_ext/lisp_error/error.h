#pragma once

#include <antlisp/lib/cell/cell_type.h>

#include <memory>
#include <string>
#include <vector>

namespace AntLisp {

class LispError;

class LispError {
public:
    using PtrType = std::shared_ptr<LispError>;
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

    static PtrType create(
        CodeType errCode
        , std::string msg
    );

    static PtrType inherit(
        PtrType fromError
        , CodeType errCode
        , std::string msg
    );

    std::string message() const;

private:
    void messageImpl(std::string& to) const;

private:
    CodeType err_code_ = defaultCode;
    std::string message_;
    PtrType from_error_;
};

using LispErrorPtr = LispError::PtrType;

using LispErrorCell = CellType<LispErrorPtr>;

template<>
std::string CellType<LispErrorPtr>::toString() const;

template<>
ICellType::Ptr CellType<LispErrorPtr>::copy() const;

}  // namespace AntLisp
