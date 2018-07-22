#include "load.h"

#include <boost/core/ignore_unused.hpp>
#include <boost/format.hpp>

#include <antlisp/lib/cell_ext/lisp_error/error.h>
#include <antlisp/lib/cell_ext/string/string.h>


namespace AntLisp {

Cell NamesLoader::instantCall(
    Arguments args
) const {
    boost::ignore_unused(args);
    return Cell::nil();
}

template<>
std::string CellType<Namespace>::toString() const {
    std::ostringstream out;
    for (const auto& name : value_) {
        out << "  {" << name.first << ' ';
        if (name.second.is<FunctionPtr>()) {
            out << "<function>";
        } else {
            out << name.second.toString();
        }
        out << "}\n";
    }
    return out.str();
}

template<>
ICellType::Ptr CellType<Namespace>::copy() const {
    // TODO
    return std::make_unique<CellType<Namespace>>(
    );
}

NamesCache::NamesCache(
    std::vector<boost::filesystem::path> paths
)
    : paths_(std::move(paths))
{
}

// void loadFromFile(
//     Arguments args
//     , Namespace& dst
// ) {
//     boost::ignore_unused(args);
//     boost::ignore_unused(dst);
// }
//
// void loadFromWeb(
//     Arguments args
//     , Namespace& dst
// ) {
//     boost::ignore_unused(args);
//     boost::ignore_unused(dst);
// }
//
// void loadBuiltins(
//     Arguments args
//     , Namespace& dst
// ) {
//     boost::ignore_unused(args);
//     boost::ignore_unused(dst);
// }

// void loadNames(
//     Arguments args
//     , Namespace& dst
// ) {
// }

}  // namespace AntLisp
