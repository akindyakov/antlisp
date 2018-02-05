#include "cell.h"

//#include <lib/tests/assert.h>

#include <iostream>

// void nilTest() {
//     std::cerr << " - nilTest\n";
//     UT_ASSERT_EQUAL(
//         Lisp::Nil{} == Lisp::Nil{},
//         true
//     );
// }
// 
// template<typename T>
// void checkTagTempl() {
//     auto v = Lisp::Cell(T{});
//     UT_ASSERT_EQUAL(
//         static_cast<int>(v.tag()),
//         static_cast<int>(Lisp::Cell::tagOf<T>())
//     );
// }
// 
// void cellCheckTypeTag() {
//     std::cerr << " - cellCheckTypeTag\n";
//     checkTagTempl<Lisp::Nil        >();
//     checkTagTempl<Lisp::Integer    >();
//     checkTagTempl<Lisp::Float      >();
//     checkTagTempl<Lisp::Symbol     >();
//     checkTagTempl<Lisp::String     >();
//     checkTagTempl<Lisp::ConsPtr    >();
// //    checkTagTempl<Lisp::Table      >();
//     checkTagTempl<Lisp::FunctionPtr>();
// }
// 
// template<typename T>
// void checkCellGet(const T& bv) {
//     auto v = Lisp::Cell(bv);
//     UT_ASSERT_EQUAL(
//         v.get<T>(),
//         bv
//     );
// }
// 
// void cellGet() {
//     std::cerr << " - cellGet\n";
//     checkCellGet(Lisp::Integer{123});
//     checkCellGet(Lisp::Float{123.01});
//     checkCellGet(Lisp::Symbol{'%'});
//     checkCellGet(Lisp::String{"What is real?"});
// }
// 
// void cellToStringTest() {
//     std::cerr << " - cellToStringTest\n";
//     UT_ASSERT_EQUAL(
//         Lisp::toString(Lisp::Integer{999223}),
//         std::string("999223")
//     );
//     UT_ASSERT_EQUAL(
//         Lisp::toString(Lisp::Float{342.341000}),
//         std::string("342.341000")
//     );
//     UT_ASSERT_EQUAL(
//         Lisp::toString(Lisp::Symbol{'@'}),
//         std::string("#\\@")
//     );
//     UT_ASSERT_EQUAL(
//         Lisp::toString(Lisp::String{"How do you define real?"}),
//         std::string("\"How do you define real?\"")
//     );
// }
int main() {
    try {
        std::cerr << "cell_ut:\n";
//       nilTest();
//       cellCheckTypeTag();
//       cellGet();
//       cellToStringTest();
        std::cerr << std::endl;
    } catch (const std::exception& except) {
        std::cerr << "failed: " << except.what() << '\n';
        return 1;
    }
    return 0;
}
