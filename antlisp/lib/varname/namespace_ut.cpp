#include "namespace.h"

#include <antlisp/lib/test/ut.h>

void set_has() {
    auto nspace = AntLisp::Namespace{};
    nspace.set("sin", AntLisp::Cell::nil());
    UT_ASSERT(nspace.has("sin"));

    nspace.set("cos", AntLisp::Cell::nil());
    UT_ASSERT(nspace.has("sin"));
    UT_ASSERT(nspace.has("cos"));
}

void set_has_remove() {
    auto nspace = AntLisp::Namespace{};
    nspace.set("sin", AntLisp::Cell::nil());
    nspace.set("cos", AntLisp::Cell::nil());
    UT_ASSERT(nspace.has("sin"));
    UT_ASSERT(nspace.has("cos"));

    nspace.remove("cos");

    UT_ASSERT(nspace.has("sin"));
    UT_ASSERT(not nspace.has("cos"));
}

void set_get() {
    auto nspace = AntLisp::Namespace{};
    nspace.set("avx1b", AntLisp::Cell::integer(384));
    nspace.set("nic", AntLisp::Cell::nil());
    {
        auto secondSpace = std::make_shared<AntLisp::Namespace>();
        secondSpace->set("second", AntLisp::Cell::integer(1701));
        nspace.set(
            "root",
            AntLisp::Cell::ext<AntLisp::NamespacePtr>(
                std::move(secondSpace)
            )
        );
    }

    UT_ASSERT_EXCEPTION_TYPE(nspace.get("notKnownName"), AntLisp::Namespace::Error);

    UT_ASSERT_EQUAL(nspace.get("nic").as<AntLisp::Nil>(), AntLisp::Nil());
    UT_ASSERT_EQUAL(nspace.get("avx1b").as<AntLisp::Integer>(), 384);
    UT_ASSERT_EQUAL(nspace.get("root:second").as<AntLisp::Integer>(), 1701);
}

UT_LIST(
    RUN_TEST(set_has);
    RUN_TEST(set_has_remove);
    RUN_TEST(set_get);
);
