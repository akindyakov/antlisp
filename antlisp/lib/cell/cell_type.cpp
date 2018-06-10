#include "cell_type.h"


namespace AntLisp {

std::ostream& operator<<(std::ostream& os, const Nil& /*v*/) {
    os << CellType<Nil>{}.toString();
    return os;
}

TypeId genTypeId() noexcept {
    static auto counter = TypeId{0}; // Nil::typeId_;
    return ++counter;
}

} // namespace AntLisp
