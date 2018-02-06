#pragma once

#include <string>


namespace AntLisp {
namespace Str {

std::string Wrap(
    const std::string& str
    , const std::string& prefix
    , const std::string& sufix
);

std::string Quotes(
    const std::string& str
);

std::string Parentheses(
    const std::string& str
);

}  // namespace Str
}  // namespace AntLisp
