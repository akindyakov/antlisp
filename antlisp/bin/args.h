#pragma once

#include <boost/optional.hpp>
#include <boost/program_options/variables_map.hpp>


using Arguments = boost::program_options::variables_map;
using ArgumentsOpt = boost::optional<Arguments>;

ArgumentsOpt Argparse(int argn, char** argv);
