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
};

template<
    typename Err
    , typename Val
    , typename = typename std::enable_if<
        std::is_convertible<
            typename std::decay<Err>::type&,
            Exception&
        >::value
    >::type
>
inline Err operator<<(
    Err&& err
    , const Val& val
) {
    std::ostringstream ostr(std::ios_base::ate);
    ostr << val;
    err.append(ostr.str());
    return std::forward<Err>(err);
}

class RuntimeError
    : public Exception
{
};

class TypeError
    : public Exception
{
};

class NotImplementedError
    : public Exception
{
};

}  // namespace AntLisp
