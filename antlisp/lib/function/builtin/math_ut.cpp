#include "math.h"

#include <antlisp/lib/test/ut.h>

#include <iostream>


void test_sum() {
    auto sum = AntLisp::Builtin::Sum();
    UT_ASSERT_EQUAL(
        sum.instantCall(
            AntLisp::Arguments{
                AntLisp::Cell::integer(192),
                AntLisp::Cell::integer(168),
                AntLisp::Cell::integer(1),
                AntLisp::Cell::integer(1),
            }
        ),
        AntLisp::Cell::integer(192 + 168 + 1 + 1)
    );
    {
        auto out = sum.instantCall(
            AntLisp::Arguments{
                AntLisp::Cell::integer(92),
                AntLisp::Cell::real(16.99),
            }
        ).get<AntLisp::Float>();
        UT_ASSERT(out < 16 + 1.0 + 92);
        UT_ASSERT(out > 16 + 0.98 + 92);
    }
    {
        auto out = sum.instantCall(
            AntLisp::Arguments{
                AntLisp::Cell::symbol('$'),
                AntLisp::Cell::string("in the"),
                AntLisp::Cell::symbol(' '),
                AntLisp::Cell::string("dark"),
                AntLisp::Cell::symbol('$'),
            }
        );
        std::cerr << out.toString() << '\n';
        UT_ASSERT_EQUAL(
            *out.get<AntLisp::StringPtr>(),
            std::string("$in the dark$")
        );
        UT_ASSERT_EQUAL(
            out,
            AntLisp::Cell::string("$in the dark$")
        );
    }
}

UT_LIST(
    RUN_TEST(test_sum);
);
