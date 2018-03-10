#include <antlisp/lib/function/stack_machine.h>


namespace AntLisp {

namespace Builtin {

class Sum
    : public ExtInstantFunction
{
public:
    Cell instantCall(
        Arguments args
    ) const override;
};

}  // namespace Builtin

}  // namespace AntLisp
