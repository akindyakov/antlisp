#pragma once

#include <antlisp/src/util/exception.h>

#include <antlisp/src/function/stack_machine.h>


namespace AntLisp {

class SyntaxError
    : public Exception
{
};

NativeFunction parseCode(
    std::istream& in
    , const Namespace& global
);

}  // namespace AntLisp
