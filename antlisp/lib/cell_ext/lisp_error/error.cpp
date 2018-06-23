#include "error.h"

#include <antlisp/lib/util/string.h>

namespace AntLisp {

constexpr LispError::CodeType LispError::defaultCode;

LispError::LispError(
    std::string msg
)
    : err_code_(1, defaultCode)
    , message_(
        std::move(msg)
    )
{
}

LispError::LispError(
    CodeType errCode
    , std::string msg
)
    : err_code_(1, errCode)
    , message_(
        std::move(msg)
    )
{
}

LispError LispError::inherit(
    const LispError& err
) {
    auto newErr = LispError(
        LispError::defaultCode,
        err.message_
    );
    newErr.err_code_.insert(
        newErr.err_code_.end(),
        err.err_code_.begin(),
        err.err_code_.end()
    );
    return newErr;
}

LispError LispError::inherit(
    const LispError& err
    , CodeType errCode
    , std::string msg
) {
    auto newErr = LispError(errCode, std::move(msg));
    newErr.err_code_.insert(
        newErr.err_code_.end(),
        err.err_code_.begin(),
        err.err_code_.end()
    );
    return newErr;
}

std::string LispError::message() const {
    auto repr = std::string{"antlisp error("};
    for (const auto& code : err_code_) {
        repr += std::to_string(code);
        repr += '.';
    }
    repr += "): ";
    repr.append(Str::Quotes(message_));
    return repr;
}


template<>
std::string CellType<LispError>::toString() const {
    return value_.message();
}

template<>
ICellType::Ptr CellType<LispError>::copy() const {
    return std::make_unique<CellType<LispError>>(
        LispError::inherit(value_)
    );
}

}  // namespace AntLisp
