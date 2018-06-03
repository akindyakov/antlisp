#pragma once

#include <antlisp/lib/cell/cell_type.h>
#include <antlisp/lib/util/string.h>

namespace AntLisp {

using StringType = std::string;
using StringCell = CellType<StringType>;

template<>
std::string CellType<StringType>::toString() const;

template<>
ICellType::Ptr CellType<StringType>::copy() const;

}  // namespace AntLisp
