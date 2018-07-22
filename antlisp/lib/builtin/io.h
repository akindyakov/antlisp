#pragma once

#include <antlisp/lib/function/tape_machine.h>


namespace AntLisp {
namespace Builtin {
namespace IO {

void all(Namespace& space);

class CoutPrint
    : public ExtInstantFunction
{
public:
    Cell instantCall(
        Arguments args
    ) const override;
};

}  // namespace IO
}  // namespace Builtin
}  // namespace AntLisp
