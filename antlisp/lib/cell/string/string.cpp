#include "string.h"

#include <antlisp/lib/util/string.h>


namespace AntLisp {

template<>
std::string CellType<StringType>::toString() const {
    return Str::Quotes(value_);
}

template<>
ICellType::Ptr CellType<StringType>::copy() const {
    return std::make_unique<StringCell>(value_);
}

}  // namespace AntLisp
