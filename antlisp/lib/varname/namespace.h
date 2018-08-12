#pragma once

#include "prefix.h"

#include <antlisp/lib/cell/cell.h>

#include <antlisp/lib/util/exception.h>
#include <antlisp/lib/util/string.h>

#include <boost/utility/string_view.hpp>

#include <string>
#include <unordered_map>


namespace AntLisp {

class Namespace;
using NamespacePtr = std::shared_ptr<Namespace>;
using NamespaceCell = CellType<NamespacePtr>;

template<>
std::string CellType<NamespacePtr>::toString() const;

template<>
ICellType::Ptr CellType<NamespacePtr>::copy() const;

class Namespace {
public:
    explicit Namespace() = default;

    Namespace(const Namespace&) = delete;
    Namespace(Namespace&& other)
        : table_(std::move(other.table_))
    {
        other.table_.clear();
    }

    Namespace& operator=(const Namespace&) = delete;
    Namespace& operator=(Namespace&& other) {
        table_ = std::move(other.table_);
        other.table_.clear();
        return *this;
    }

    class Error
        : public Exception
    {
    };

    /**
     * Depricated, for backward compatiblity
     * Use set() instead
     */
    void emplace(const VarName& name, Cell cell);

    /**
     * Depricated, for backward compatiblity
     * Use set() instead
     */
    void insert(const VarName& name, Cell cell);

    void set(const VarName& name, Cell cell);

    /**
     * Depricated, for backward compatiblity
     * Use get() instead
     */
    Cell const& at(const VarName& name);

    Cell const& get(const VarName& name) const;

    Cell const& get(VarNamePrefix prefix) const;

    bool has(const VarName& name) const;

    void remove(const VarName& name);

    Namespace copy() const;

    void update(const Namespace& other);

    std::string toString() const;

private:
    std::unordered_map<VarName, Cell> table_;
};

}  // namespace AntLisp
