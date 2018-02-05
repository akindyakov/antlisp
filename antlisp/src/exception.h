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
        typename StrType
    >
    void append(StrType&& str) {
        message_.append(
            std::forward<StrType>(str)
        );
    }

    template<
        typename Type
    >
    Exception&& operator()(
        const Type& value
    ) {
        std::ostringstream ostr;
        ostr << value;
        this->append(
            ostr.str()
        );
        return std::move(*this);
    }
};

}  // namespace AntLisp
