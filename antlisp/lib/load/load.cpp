#include "load.h"

#include <boost/core/ignore_unused.hpp>


namespace AntLisp {
namespace Builtin {

void loadLoad(Namespace& space) {
    boost::ignore_unused(space);
}

Cell Load::instantCall(
    Arguments args
) const {
    boost::ignore_unused(args);
    return Cell::nil();
}

}  // namespace Builtin
}  // namespace AntLisp
