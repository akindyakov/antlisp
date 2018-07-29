#include "prefix.h"

namespace AntLisp {

VarNamePrefix::VarNamePrefix(
    boost::string_view prefix
)
    : prefix_(std::move(prefix))
    , name_(prefix_)
{
    const auto pos = name_.find(VarNamePrefix::separator);
    if (pos != std::string::npos) {
        name_.remove_suffix(name_.size() - pos);
    }
}

std::string::value_type constexpr VarNamePrefix::separator;

VarNamePrefix VarNamePrefix::next() const
{
    auto nextPrefix = prefix_;  // copy
    nextPrefix.remove_prefix(name_.size() + 1);
    return VarNamePrefix(nextPrefix);
}

}  // namespace AntLisp {
