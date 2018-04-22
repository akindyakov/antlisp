#pragma once

#include <antlisp/lib/cell/cell.h>


namespace AntLisp {

class MockExtType
    : public IExtType
{
public:
    std::string toString() const override;
    ExtTypePtr copy() const override;

    void summarize(const Cell&) override;
    void multiply(const Cell&) override;
    void subtract(const Cell&) override;
    void divide(const Cell&) override;

    bool equal(const Cell&) const override;
    bool less(const Cell&) const override;
};

}  // namespace AntLisp
