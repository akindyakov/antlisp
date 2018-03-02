#include "args.h"

#include <boost/optional/optional_io.hpp>
#include <boost/program_options/options_description.hpp>
#include <boost/program_options/parsers.hpp>
#include <boost/program_options/variables_map.hpp>

#include <iostream>


namespace Args = boost::program_options;

ArgumentsOpt Argparse(int argn, char** argv) {
    auto general = Args::options_description{"Options"};
    general.add_options()
        ("cmd,c", "program passed in as string")
        (
            "strip-docstrings",
            Args::value<bool>()->default_value(false),
            "remove doc-strings from translated code."
        )
        ("help,h", "produce help message")
        ("version,v", "print version string")
    ;
    auto hidden = Args::options_description{"Options"};
    hidden.add_options()
        (
            "script-file",
            "read script from file, use '-' to read script from stdin."
        )
    ;
    auto posOptions = Args::positional_options_description{};
    posOptions.add("script-file", 1);

    auto allOptions = Args::options_description{};
    allOptions.add(general).add(hidden);

    auto args = Args::variables_map{};
    Args::store(
        Args::command_line_parser(argn, argv)
            .options(general)
            .positional(posOptions)
            .run(),
        args
    );
    Args::notify(args);

    auto argsOpt = ArgumentsOpt{};
    if (args.count("help")) {
        std::cout << "Positional arguments:\n";
        for (
            auto pos = decltype(posOptions.max_total_count()){0}
            ; pos < posOptions.max_total_count(); ++pos
        ) {
            const auto& name = posOptions.name_for_position(pos);
            const auto& opt = hidden.find(name, false);
            std::cout << "  " << name << "\n " << opt.description() << "\n";
        }
        std::cout << "\n";
        general.print(std::cout, 1);
    } else {
        argsOpt.reset(args);
    }
    return argsOpt;
}
