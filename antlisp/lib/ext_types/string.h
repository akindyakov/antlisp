#pragma once

#include <antlisp/lib/cell/cell.h>
#include <antlisp/lib/util/exception.h>

#include <boost/variant.hpp>
#include <boost/variant/get.hpp>
#include <boost/optional.hpp>

#include <string>
#include <unordered_map>


namespace AntLisp {

class StringType
    : public MockExtType
{
public:
    explicit StringType() = default;

    template<
        typename SomeOtherStringType
    >
    explicit StringType(SomeOtherStringType&& v)
        : value(
            std::forward<SomeOtherStringType>(v)
        )
    {
    }

    std::string toString() const override;
    ExtTypePtr copy() const override;

    void summarize(const Cell&) override;
    void multiplication(const Cell&) override;

private:
    std::string value;
};

}  // namespace AntLisp
