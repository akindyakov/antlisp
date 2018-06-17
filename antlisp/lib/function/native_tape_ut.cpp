#include "native_tape.h"

#include <antlisp/lib/test/ut.h>

#include <iostream>


void testNativeTapeToString() {
    auto tape = AntLisp::NativeTape{};
    auto str = tape.toString();
    auto pos = std::size_t{0};
    pos = str.find("names", pos);
    UT_ASSERT(
        pos != std::string::npos
    );
    pos = str.find("consts", pos);
    UT_ASSERT(
        pos != std::string::npos
    );
    pos = str.find("steps", pos);
    UT_ASSERT(
        pos != std::string::npos
    );
}

void testNativeTapeOperationsPrint() {
    using EInt = std::underlying_type<AntLisp::NativeTape::EOperations>::type;
    auto upLimit = static_cast<EInt>(AntLisp::NativeTape::InvalidUpLimit);
    for (auto i = EInt{}; i < upLimit; ++i) {
        auto op = static_cast<AntLisp::NativeTape::EOperations>(i);
        UT_ASSERT(
            AntLisp::toString(op).size() > 0
        );
    }
}

UT_LIST(
    RUN_TEST(testNativeTapeToString);
    RUN_TEST(testNativeTapeOperationsPrint);
);
