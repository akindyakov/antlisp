#include <antlisp/lib/function/tape_machine.h>


namespace AntLisp {

namespace Builtin {

void allMathFunctions(Namespace& space);

class Sum
    : public ExtInstantFunction
{
public:
    Cell instantCall(
        Arguments args
    ) const override;
};

class Multiplication
    : public ExtInstantFunction
{
public:
    Cell instantCall(
        Arguments args
    ) const override;
};

class Division
    : public ExtInstantFunction
{
public:
    Cell instantCall(
        Arguments args
    ) const override;
};

class Less
    : public ExtInstantFunction
{
public:
    Cell instantCall(
        Arguments args
    ) const override;
};

class Equality
    : public ExtInstantFunction
{
public:
    Cell instantCall(
        Arguments args
    ) const override;
};

}  // namespace Builtin

}  // namespace AntLisp
