#pragma once

#include <antlisp/lib/cell/ext/mock.hpp>
#include <antlisp/lib/name/name.hpp>


namespace AntLisp {

class ArrayType
    : public MockExtType
{
public:
    explicit ArrayType() = default;

    static void publish(
        Namespace& space
    );
};

}  // namespace AntLisp
