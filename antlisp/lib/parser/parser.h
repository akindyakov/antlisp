#pragma once

#include <antlisp/lib/util/exception.h>

#include <antlisp/lib/function/stack_machine.h>


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
