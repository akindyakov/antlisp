#include "prefix.h"

namespace AntLisp {

VarNamePrefix::VarNamePrefix(
    boost::string_view prefix
)
    : fullName_(std::move(prefix))
    , first_(fullName_)
{
    const auto pos = fullName_.find(VarNamePrefix::separator);
    if (pos != std::string::npos) {
        first_.remove_suffix(fullName_.size() - pos);
    }
}

std::string::value_type constexpr VarNamePrefix::separator;

VarNamePrefix VarNamePrefix::next() const
{
    auto nextPrefix = fullName_;  // copy
    nextPrefix.remove_prefix(first_.size() + 1);
    return VarNamePrefix(nextPrefix);
}

}  // namespace AntLisp {
