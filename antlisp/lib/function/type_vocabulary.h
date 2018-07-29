#pragma once

#include <antlisp/lib/cell/cell.h>

#include <antlisp/lib/util/exception.h>

#include <string>
#include <unordered_map>
#include <vector>


namespace AntLisp {

using VarName = std::string;

using Namespace = std::unordered_map<VarName, Cell>;

using Arguments = std::vector<Cell>;

using ArgNames = std::vector<VarName>;

class TapeMachineError
    : public Exception
{
};

}  // namespace AntLisp
