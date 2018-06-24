#include "load.h"

#include <boost/core/ignore_unused.hpp>


namespace AntLisp {

NamesLoader::NamesLoader(
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
