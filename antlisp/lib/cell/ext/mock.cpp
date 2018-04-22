#include "mock.hpp"

#include <antlisp/lib/util/exception.h>


namespace AntLisp {

std::string MockExtType::toString() const {
    std::cerr << "mock MockExtType::toString()\n";
    throw NotImplementedError() << "\"toString\" method does not implemented";
}

ExtTypePtr MockExtType::copy() const {
    throw NotImplementedError() << "\"copy\" method does not implemented";
}
void MockExtType::summarize(const Cell&) {
    throw NotImplementedError() << "\"summarize\" method does not implemented";
}
void MockExtType::multiply(const Cell&) {
    throw NotImplementedError() << "\"multiply\" method does not implemented";
}
void MockExtType::subtract(const Cell&) {
    throw NotImplementedError() << "\"subtract\" method does not implemented";
}
void MockExtType::divide(const Cell&) {
    throw NotImplementedError() << "\"divide\" method does not implemented";
}

bool MockExtType::equal(const Cell&) const {
    throw NotImplementedError() << "\"equal\" method does not implemented";
}
bool MockExtType::less(const Cell&) const {
    throw NotImplementedError() << "\"less\" method does not implemented";
}

}  // namespace AntLisp
