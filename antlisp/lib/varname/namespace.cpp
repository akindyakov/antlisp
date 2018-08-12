#include "namespace.h"

namespace AntLisp {

template<>
std::string CellType<NamespacePtr>::toString() const {
    return value_->toString();
}

template<>
ICellType::Ptr CellType<NamespacePtr>::copy() const {
    return std::make_unique<CellType<NamespacePtr>>(
        std::make_shared<Namespace>(value_->copy())
    );
}

/**
 * Depricated, for backward compatiblity
 * Use set() instead
 */
void Namespace::emplace(const VarName& name, Cell cell) {
    set(name, std::move(cell));
}

/**
 * Depricated, for backward compatiblity
 * Use set() instead
 */
void Namespace::insert(const VarName& name, Cell cell) {
    set(name, std::move(cell));
}

void Namespace::set(const VarName& name, Cell cell) {
    auto it = table_.find(name);
    if (it == table_.end()) {
        std::cerr << "set [" << name << "] " << cell.toString() << '\n';
        table_.emplace(name, std::move(cell));
    } else {
        it->second = std::move(cell);
    }
}

/**
 * Depricated, for backward compatiblity
 * Use get() instead
 */
Cell const& Namespace::at(const VarName& name) {
    return get(name);
}

Cell const& Namespace::get(const VarName& name) const {
    return get(
        VarNamePrefix(name)
    );
}

Cell const& Namespace::get(VarNamePrefix prefix) const {
  std::cerr << prefix.firstName() << '\n';
    auto const it = table_.find(
        prefix.firstName()
    );
    if (it == table_.end()) {
        throw Error()
            << "No such suffix "
            << Str::Quotes(prefix.fullName())
            << " in namespace";
    }
    if (not prefix.isComplex()) {
        return it->second;
    }
    if (not it->second.is<NamespacePtr>()) {
        throw Error()
            << "Look up in the namespace for suffix "
            << Str::Quotes(prefix.fullName())
            << "failed: Cell is not a namespace";
    }
    return it->second.as<NamespacePtr>()->get(prefix.next());
}

bool Namespace::has(const VarName& name) const {
    return table_.find(name) != table_.end();
}

void Namespace::remove(const VarName& name) {
    table_.erase(name);
}

Namespace Namespace::copy() const {
    auto newNamespace = Namespace{};
    for (const auto& var : table_) {
        newNamespace.emplace(var.first, var.second.copy());
    }
    return newNamespace;
}

void Namespace::update(const Namespace& other) {
    for (const auto& var : other.table_) {
        set(var.first, var.second.copy());
    }
}

std::string Namespace::toString() const {
    auto str = std::string{};
    for (const auto& var : table_) {
        str.append("  {");
        str.append(var.first);
        str.push_back(' ');
        if (var.second.is<FunctionPtr>()) {
            str.append("<function>");
        } else {
            str.append(
                var.second.toString()
            );
        }
        str.append("}\n");
    }
    return str;
}

}  // namespace AntLisp {
