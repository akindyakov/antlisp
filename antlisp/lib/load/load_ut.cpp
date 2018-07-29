#include "load.h"

#include <antlisp/lib/cell_ext/string/string.h>

#include <antlisp/lib/test/ut.h>

void simple_io() {
    auto loader = AntLisp::NamesLoader{};
    auto args = AntLisp::Arguments{};
    args.push_back(
        AntLisp::Cell::ext<AntLisp::StringType>("io")
    );
    auto ret = loader.instantCall(std::move(args));
    // UT_ASSERT(ret.is<AntLisp::NamespacePtr>());
    // const auto namesPtr = ret.as<AntLisp::NamespacePtr>().get();
    // const auto& print = namesPtr->at("print");
    // UT_ASSERT(print.is<AntLisp::FunctionPtr>());
}

UT_LIST(
    RUN_TEST(simple_io);
);
