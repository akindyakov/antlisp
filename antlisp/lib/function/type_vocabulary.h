#pragma once

#include <antlisp/lib/cell/cell.h>
#include <antlisp/lib/varname/varname.h>

#include <antlisp/lib/util/exception.h>

#include <vector>


namespace AntLisp {

using Arguments = std::vector<Cell>;

using ArgNames = std::vector<VarName>;

class TapeMachineError
    : public Exception
{
};

}  // namespace AntLisp
