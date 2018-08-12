#pragma once

#include "prefix.h"

#include <string_view>

namespace AntLisp {

using VarNameLink = std::string_view;

class NamePool {
public:
    explicit NamePool() = default;

    std::size_t store(
        VarNameLink name
    );

    std::size_t store(
        VarName name
    );

    std::size_t store(
        VarName name
    );

    std::size_t getIndex(
        VarNameLink name
    ) const;

    VarNameLink getSymlink(
        std::size_t pos
    ) const;

private:
    std::unordered_map<VarNameLink, std::size_t> index_;
    std::vector<VarName> names_;
};

}
