#pragma once

#include <antlisp/lib/cell/cell.h>

#include <antlisp/lib/util/exception.h>

#include <boost/utility/string_view.hpp>

#include <string>
#include <unordered_map>


namespace AntLisp {

using VarName = std::string;

using Namespace = std::unordered_map<VarName, Cell>;

class VarNamePrefix {
public:
    explicit VarNamePrefix(
        boost::string_view prefix
    );

    VarNamePrefix next() const;

    static std::string::value_type constexpr separator = ':';

    bool isValid() const {
        return not name_.empty();
    }

    VarName fullName() const {
        return prefix_.to_string();
    }

    VarName firstName() const {
        return name_.to_string();
    }

private:
    boost::string_view prefix_;
    boost::string_view name_;
};

}  // namespace AntLisp
