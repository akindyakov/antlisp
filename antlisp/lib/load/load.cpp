#include "load.h"

#include <boost/core/ignore_unused.hpp>
#include <boost/format.hpp>

#include <antlisp/lib/builtin/io.h>
#include <antlisp/lib/cell_ext/lisp_error/error.h>
#include <antlisp/lib/cell_ext/string/string.h>


namespace AntLisp {

Cell NamesLoader::cell() {
    return Cell::function(
        std::make_shared<NamesLoader>()
    );
}

void NamesLoader::inject(Namespace& space) {
    space.emplace("load", NamesLoader::cell());
}

Cell NamesLoader::instantCall(
    Arguments args
) const {
    if (args.size() == 0 || args.size() > 2) {
        throw TypeError() << "Load function takes 1 or 2 arguments";
    }
    if (args.size() == 1) {
        if (not args[0].is<StringType>()) {
            throw TypeError()
                << "Wrong type of module name " << args[0].toString();
        }
        const auto& module = args[0].cAs<StringType>();
        if (module == "io") {
            auto namesCell = Cell::ext<NamespacePtr>(
                std::make_shared<Namespace>()
            );
            auto namesPtr = namesCell.as<NamespacePtr>().get();
            Builtin::IO::all(*namesPtr);
            return namesCell;
        } else {
            throw TypeError()
                << "unknown builtin module " << Str::Quotes(module);
        }
    } else {
        throw NotImplementedError()
            << "Load from antlisp source files is not implemented yet";
    }
    return Cell::nil();
}

template<>
std::string CellType<NamespacePtr>::toString() const {
    std::ostringstream out;
    out << "(namespace ";
    const auto namesPtr = value_.get();
    for (const auto& name : *namesPtr) {
        out << "(" << name.first << ' ';
        if (name.second.is<FunctionPtr>()) {
            out << "function-ptr";
        } else {
            out << name.second.toString();
        }
        out << ") ";
    }
    out << ")";
    return out.str();
}

template<>
ICellType::Ptr CellType<NamespacePtr>::copy() const {
    return std::make_unique<CellType<NamespacePtr>>(
        value_
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
