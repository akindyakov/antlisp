#pragma once

#include <antlisp/lib/util/exception.h>


namespace AntLisp {

class ParseError
    : public Exception
{
};

class NameError
    : public ParseError
{
};

class SyntaxError
    : public ParseError
{
};

}  // namespace AntLisp
