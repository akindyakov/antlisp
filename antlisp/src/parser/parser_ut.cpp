#include "parser.h"

#include <antlisp/src/test/ut.h>

#include <sstream>


void testParenthesesRecursiveReader() {
    std::istringstream in("  (sum (* 2) (+ 5) 15) ");
    auto reader = AntLisp::InCodeStream(in);
    UT_ASSERT(
        reader.good()
    );
    auto parser1 = AntLisp::ParenthesesParser::openFromCodeStream(reader);
    UT_ASSERT(
        reader.good()
    );
    auto token = std::string();
    UT_ASSERT(
        parser1.nextToken(token)
    );
    UT_ASSERT_EQUAL(token, "sum");
    UT_ASSERT(
        !parser1.nextToken(token)
    );
    UT_ASSERT(
        reader.good()
    );
    UT_ASSERT(
        !parser1.good()
    );
    UT_ASSERT(
        parser1.isLocked()
    );

    auto parser2 = parser1.nextParser();
    UT_ASSERT(
        parser2.nextToken(token)
    );
    UT_ASSERT_EQUAL(token, "*");
    UT_ASSERT(
        parser2.nextToken(token)
    );
    UT_ASSERT_EQUAL(token, "2");
    UT_ASSERT(
        !parser2.nextToken(token)
    );
    UT_ASSERT(
        !parser2.good()
    );
    UT_ASSERT(
        !parser2.isLocked()
    );

    UT_ASSERT(
        parser1.good()
    );
    UT_ASSERT(
        !parser1.isLocked()
    );
    UT_ASSERT(
        !parser1.nextToken(token)
    );

    auto parser3 = parser1.nextParser();
    UT_ASSERT(
        parser3.nextToken(token)
    );
    UT_ASSERT_EQUAL(token, "+");
    UT_ASSERT(
        parser3.nextToken(token)
    );
    UT_ASSERT_EQUAL(token, "5");
    UT_ASSERT(
        !parser3.nextToken(token)
    );
    UT_ASSERT(
        !parser3.good()
    );

    UT_ASSERT(
        parser1.good()
    );
    UT_ASSERT(
        !parser1.isLocked()
    );
    UT_ASSERT(
        parser1.nextToken(token)
    );
    UT_ASSERT_EQUAL(token, "15");
}

void test_parseCode() {
    auto global = AntLisp::Namespace{
        {"*", AntLisp::Cell(std::make_shared<AntLisp::ExtMultiplication>())},
        {"sum", AntLisp::Cell(std::make_shared<AntLisp::ExtSum>())},
    };
    std::istringstream in("  (sum 1.23 (* 2 3)) ");
    auto lambda = AntLisp::parseCode(in, global);
    auto nativeDef = lambda->core();
    UT_ASSERT_EQUAL(nativeDef->consts.size(), 3);
    UT_ASSERT_EQUAL(nativeDef->names.size(), 2);
}

void test_parseCode_lambda() {
    auto global = AntLisp::Namespace{
        {"+", AntLisp::Cell(std::make_shared<AntLisp::ExtSum>())},
    };
    std::istringstream in("(lambda (x) (+ x 1))");
    auto lambda = AntLisp::parseCode(in, global);
    UT_ASSERT_EQUAL(lambda->names.size(), 0);
    auto nativeDef = lambda->core();
    UT_ASSERT_EQUAL(nativeDef->consts.size(), 1);
    UT_ASSERT_EQUAL(nativeDef->names.size(), 1);
}

void test_parseCode_defun() {
    auto global = AntLisp::Namespace{
        {"+", AntLisp::Cell(std::make_shared<AntLisp::ExtSum>())},
    };
    std::istringstream in("(defun rinzler (x) (+ x 1 2))");
    auto lambda = AntLisp::parseCode(in, global);
    UT_ASSERT_EQUAL(lambda->names.size(), 0);
    auto nativeDef = lambda->core();
    UT_ASSERT_EQUAL(nativeDef->consts.size(), 1);
    UT_ASSERT_EQUAL(nativeDef->names.size(), 2);  // + and rinzler
}

void test_parseCode_cond() {
    auto global = AntLisp::Namespace{
        {"+", AntLisp::Cell(std::make_shared<AntLisp::ExtSum>())},
    };
    std::istringstream in(R"antlisp-code(
    (cond
        (nil  (+ 1 2))
        (true (+ 2 3))
        ((+ 1 0) (+ 3 4))
    )
    )antlisp-code");
    auto lambda = AntLisp::parseCode(in, global);
    UT_ASSERT_EQUAL(lambda->names.size(), 0);
    auto nativeDef = lambda->core();
    UT_ASSERT_EQUAL(nativeDef->consts.size(), 10);
    //std::cerr << "(names.size " << nativeDef->names.size() << ")\n";
    //for (const auto& c : nativeDef->names) {
    //    std::cerr << "(name " << c << ")\n";
    //}
    UT_ASSERT_EQUAL(nativeDef->names.size(), 1);  // +
}

UT_LIST(
    testParenthesesRecursiveReader();
    test_parseCode();
    test_parseCode_lambda();
    test_parseCode_defun();
    test_parseCode_cond();
);
