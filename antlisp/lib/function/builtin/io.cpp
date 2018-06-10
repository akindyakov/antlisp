#include "io.h"

#include <antlisp/lib/util/exception.h>

#include <iostream>


namespace AntLisp {
namespace Builtin {
namespace IO {

Cell CoutPrint::instantCall(
    Arguments args
) const {
    for (const auto& arg : args) {
        std::cout << arg.toString();
    }
    std::cout << std::endl;
    return Cell::nil();
}

void all(Namespace& space) {
    space.emplace(
        "print",
        Cell::function(
            std::make_shared<CoutPrint>()
        )
    );
}

}  // namespace IO
}  // namespace Builtin
}  // namespace AntLisp
