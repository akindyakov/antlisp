#pragma once

#include <antlisp/lib/cell/cell.h>

#include <antlisp/lib/util/exception.h>

#include <boost/utility/string_view.hpp>

#include <string>
#include <unordered_map>


namespace AntLisp {

using VarName = std::string;

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

    bool isComplex() const {
        return name_.size() != first_.size();
    }

    VarName fullName() const {
        return name_.to_string();
    }

    VarName firstName() const {
        return first_.to_string();
    }

private:
    boost::string_view name_;
    boost::string_view first_;
};

// TODO: continue here, we definetely need more complex class Namespace
using Namespace = std::unordered_map<VarName, Cell>;

}  // namespace AntLisp
