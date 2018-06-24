#include <antlisp/lib/function/type_vocabulary.h>

#include <boost/core/ignore_unused.hpp>
#include <boost/filesystem.hpp>

#include <vector>
#include <unordered_map>

namespace AntLisp {

class NamesLoader {
public:
    explicit NamesLoader() = default;
    explicit NamesLoader(
        std::vector<boost::filesystem::path> paths
    );

    /**
    * `CallableType` is callable object with arguments:
    *   0: `const TVarName& name`
    *   1: `Cell cell`
    */
    template<
        typename CallableType
    >
    Namespace load(
        Arguments args
        , CallableType fn
    ) {
        boost::ignore_unused(args);
        boost::ignore_unused(fn);
        // if (args.size() == 1) {
        // } else if (args.size() == 2) {
        // } else {
        // }
        return Namespace{};
    }

private:
    std::vector<boost::filesystem::path> paths_;
    // cache
    std::unordered_map<TVarName, Namespace> builtins_;
    //std::unordered_map<boost::filesystem::path, Namespace> cache_;
};

}  // namespace AntLisp
