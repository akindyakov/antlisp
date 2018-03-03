#include "string.h"

#include <sstream>

namespace AntLisp {
namespace Str {

std::string Wrap(
    const std::string& str
    , const std::string& prefix
    , const std::string& sufix
) {
    std::ostringstream ostr(std::ios_base::ate);
    ostr << prefix << str << sufix;
    return ostr.str();
}

std::string Quotes(
    const std::string& str
) {
    const auto quote = std::string{"\""};
    return Wrap(str, quote, quote);
}

std::string Parentheses(
    const std::string& str
) {
    return Wrap(str, "(", ")");
}

}  // namespace Str
}  // namespace AntLisp
