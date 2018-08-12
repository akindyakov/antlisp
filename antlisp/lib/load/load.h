#pragma once

#include <antlisp/lib/function/tape_machine.h>
#include <antlisp/lib/cell/cell_type.h>

#include <boost/core/ignore_unused.hpp>
#include <boost/filesystem.hpp>

#include <vector>
#include <unordered_map>


namespace AntLisp {

class NamesLoader
    : public ExtInstantFunction
{
public:
    Cell instantCall(
        Arguments args
    ) const override;

    static Cell cell();
    static void inject(Namespace& space);
};

class NamesCache {
public:
    using LoaderCallbackType = std::function<
        void(const VarName& name, Cell cell)
    >;

public:
    explicit NamesCache() = default;
    explicit NamesCache(
        std::vector<boost::filesystem::path> paths
    );

    Cell load(
        Arguments args
        , LoaderCallbackType fn
    );

private:
    std::vector<boost::filesystem::path> paths_;
    // cache
    // std::unordered_map<VarName, LoaderCallbackType> builtins_;
    // std::unordered_map<boost::filesystem::path, Namespace> cache_;
};

}  // namespace AntLisp
