#include "error.h"

#include <antlisp/lib/util/string.h>


namespace AntLisp {

constexpr LispError::CodeType LispError::defaultCode;

LispError::LispError(
    std::string msg
)
    : err_code_(defaultCode)
    , message_(
        std::move(msg)
    )
{
}

LispError::LispError(
    CodeType errCode
    , std::string msg
)
    : err_code_(errCode)
    , message_(
        std::move(msg)
    )
{
}

LispErrorPtr LispError::create(
    CodeType errCode
    , std::string msg
) {
    return std::make_shared<LispError>(errCode, std::move(msg));
}

LispErrorPtr LispError::inherit(
    LispErrorPtr fromError
    , CodeType errCode
    , std::string msg
) {
    auto newErr = LispError::create(errCode, std::move(msg));
    newErr->from_error_ = std::move(fromError);
    return newErr;
}

void LispError::messageImpl(std::string& to) const {
    to += "(error: ";
    to += std::to_string(this->err_code_);
    to += " ";
    to += Str::Quotes(this->message_);
    to += ")\n";
    if (this->from_error_) {
        this->from_error_->messageImpl(to);
    }
}

std::string LispError::message() const {
    auto repr = std::string{};
    this->messageImpl(repr);
    return repr;
}


template<>
std::string CellType<LispErrorPtr>::toString() const {
    return value_->message();
}

template<>
ICellType::Ptr CellType<LispErrorPtr>::copy() const {
    return std::make_unique<CellType<LispErrorPtr>>(
        value_
    );
}

}  // namespace AntLisp
