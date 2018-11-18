#include "exception.h"

#include <antlisp/lib/test/ut.h>

void exception() {
    auto ex = AntLisp::Exception() << "abc" << 123 << "]";
    auto expected = std::string("abc123]");
    if (ex.message() != expected) {
        std::cerr
            << "Wrong value '" << ex.message()
            << "', expected '" << expected << "'. ";
        throw std::exception();
    }
}

UT_LIST(
    RUN_TEST(exception);
);
