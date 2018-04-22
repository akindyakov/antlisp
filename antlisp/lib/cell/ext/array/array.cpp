#include "array.hpp"


namespace AntLisp {

void ArrayType::publish(
    Namespace& space
) {
    space.emplace("array.create", Cell::nil());
    space.emplace("array.append", Cell::nil());
    space.emplace("array.get", Cell::nil());
    space.emplace("array.set", Cell::nil());
    space.emplace("array.map", Cell::nil());
    space.emplace("array.for-each", Cell::nil());
}

}  // namespace AntLisp
