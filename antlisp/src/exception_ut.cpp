#include "antlisp/src/exception.h"

int main() {
    auto ex = AntLisp::Exception() << "abc" << 123 << "]";
    auto expected = std::string("abc123]");
    if (ex.message() != expected) {
        std::cerr
            << "Wrong value '" << ex.message()
            << "', expected '" << expected << "'. ";
        throw std::exception();
    }
    return 0;
}
