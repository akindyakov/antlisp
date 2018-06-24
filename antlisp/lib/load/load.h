#include <antlisp/lib/function/tape_machine.h>


namespace AntLisp {
namespace Builtin {

void loadLoad(Namespace& space);

class Load
    : public ExtInstantFunction
{
public:
    Cell instantCall(
        Arguments args
    ) const override;
};

}  // namespace Builtin
}  // namespace AntLisp
