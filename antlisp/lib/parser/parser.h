#pragma once

#include <antlisp/lib/util/exception.h>

#include <antlisp/lib/function/tape_machine.h>

#include "error.h"


namespace AntLisp {

NativeFunction parseCode(
    std::istream& in
    , const Namespace& global
);

}  // namespace AntLisp
