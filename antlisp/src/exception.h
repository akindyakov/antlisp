#pragma once

#include <exception>
#include <string>
#include <sstream>
#include <iostream>


namespace AntLisp {

class Exception
    : public std::exception
{
private:
    std::string message_;

public:
    explicit Exception() = default;

    explicit Exception(
        std::string msg
    )
        : message_(std::move(msg))
    {
    }

    virtual ~Exception() = default;

    const char* what() const throw() override {
        return message_.c_str();
    }

    const std::string& message() const noexcept{
        return message_;
    }

    template<
        typename Type
    >
    Exception&& operator()(
        const Type& value
    ) {
        std::ostringstream ostr;
        ostr << value;
        message_.append(
            ostr.str()
        );
        return std::move(*this);
    }
};

}  // namespace AntLisp
