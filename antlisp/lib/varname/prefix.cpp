#include "prefix.h"

namespace AntLisp {

VarNamePrefix::VarNamePrefix(
    boost::string_view prefix
)
    : name_(std::move(prefix))
    , first_(name_)
{
    const auto pos = name_.find(VarNamePrefix::separator);
    if (pos != std::string::npos) {
        first_.remove_suffix(name_.size() - pos);
    }
}

std::string::value_type constexpr VarNamePrefix::separator;

VarNamePrefix VarNamePrefix::next() const
{
    auto nextPrefix = name_;  // copy
    nextPrefix.remove_prefix(first_.size() + 1);
    return VarNamePrefix(nextPrefix);
}

}  // namespace AntLisp {
