#pragma once

#include <antlisp/lib/cell/cell.h>

#include <antlisp/lib/util/exception.h>

#include <string>
#include <unordered_map>
#include <vector>


namespace AntLisp {

using TVarName = std::string;

using Namespace = std::unordered_map<TVarName, Cell>;

using Arguments = std::vector<Cell>;

using ArgNames = std::vector<TVarName>;

class TapeMachineError
    : public Exception
{
};

}  // namespace AntLisp
