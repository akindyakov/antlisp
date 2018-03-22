#pragma once

#include <boost/optional.hpp>


namespace AntLisp {

template<
    typename Type
>
using Optional = boost::optional<Type>;

}  // namespace AntLisp
