#pragma once

#include <antlisp/lib/cell/cell.h>

#include <string>
#include <unordered_map>


namespace AntLisp {

using TVarName = std::string;

using Namespace = std::unordered_map<TVarName, Cell>;

}  // namespace AntLisp
