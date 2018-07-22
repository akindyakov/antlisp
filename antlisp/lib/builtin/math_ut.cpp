#include "math.h"

#include <antlisp/lib/test/ut.h>

#include <iostream>


void test_allMathBuiltinFunctionsPlus() {
    auto space = AntLisp::Namespace{};
    AntLisp::Builtin::allMathFunctions(space);
    const auto sumPtr = space.at("+").as<AntLisp::FunctionPtr>();
    auto args = AntLisp::Arguments{};
    args.push_back(AntLisp::Cell::integer(17));
    args.push_back(AntLisp::Cell::integer(16));
    UT_ASSERT_EQUAL(
        sumPtr->instantCall(
            std::move(args)
        ).as<AntLisp::Integer>(),
        17 + 16
    );
}

void test_allMathBuiltinFunctionsLess() {
    auto space = AntLisp::Namespace{};
    AntLisp::Builtin::allMathFunctions(space);
    const auto lessPtr = space.at("<").as<AntLisp::FunctionPtr>();
    auto args2 = AntLisp::Arguments{};
    args2.push_back(AntLisp::Cell::real(1.29));
    args2.push_back(AntLisp::Cell::real(1.31));
    UT_ASSERT_EQUAL(
        lessPtr->instantCall(
            std::move(args2)
        ).as<AntLisp::Integer>(),
        AntLisp::Cell::t().as<AntLisp::Integer>()
    );
}

void test_allMathBuiltinFunctionsEqual() {
    auto space = AntLisp::Namespace{};
    AntLisp::Builtin::allMathFunctions(space);
    const auto eqPtr = space.at("=").as<AntLisp::FunctionPtr>();
    auto args = AntLisp::Arguments{};
    args.push_back(AntLisp::Cell::integer(129));
    args.push_back(AntLisp::Cell::integer(129));
    auto ans = eqPtr->instantCall(
        std::move(args)
    );
    UT_ASSERT(
        ans.is<AntLisp::Integer>()
    );
}

void test_allMathBuiltinFunctionsMultiplication() {
    auto space = AntLisp::Namespace{};
    AntLisp::Builtin::allMathFunctions(space);
    const auto mPtr = space.at("*").as<AntLisp::FunctionPtr>();
    auto args = AntLisp::Arguments{};
    args.push_back(AntLisp::Cell::integer(6));
    args.push_back(AntLisp::Cell::integer(29));
    UT_ASSERT_EQUAL(
        mPtr->instantCall(
            std::move(args)
        ).as<AntLisp::Integer>(),
        AntLisp::Integer{6 * 29}
    );
}

void test_allMathBuiltinFunctionsDivision() {
    auto space = AntLisp::Namespace{};
    AntLisp::Builtin::allMathFunctions(space);
    const auto dPtr = space.at("/").as<AntLisp::FunctionPtr>();
    auto args = AntLisp::Arguments{};
    args.push_back(AntLisp::Cell::integer(143));
    args.push_back(AntLisp::Cell::integer(11));
    UT_ASSERT_EQUAL(
        dPtr->instantCall(
            std::move(args)
        ).as<AntLisp::Integer>(),
        AntLisp::Integer{13}
    );
}

UT_LIST(
    RUN_TEST(test_allMathBuiltinFunctionsPlus);
    RUN_TEST(test_allMathBuiltinFunctionsDivision);
    RUN_TEST(test_allMathBuiltinFunctionsMultiplication);
    RUN_TEST(test_allMathBuiltinFunctionsLess);
    RUN_TEST(test_allMathBuiltinFunctionsEqual);
);
