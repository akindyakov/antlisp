#include "args.h"

#include <boost/optional/optional_io.hpp>
#include <boost/program_options/options_description.hpp>
#include <boost/program_options/parsers.hpp>
#include <boost/program_options/variables_map.hpp>

#include <iostream>


namespace Bpo = boost::program_options;

ArgumentsOpt Argparse(int argn, char** argv) {
    auto general = Bpo::options_description{"Options"};
    general.add_options()
        (
            "cmd,c",
            Bpo::value<std::string>(),
            "program passed in as string"
        ) (
            "print-global",
            Bpo::value<bool>()->zero_tokens(),
            "pring global namespace after execution"
        ) (
            "help,h",
            Bpo::value<bool>()->zero_tokens(),
            "produce help message"
        ) (
            "version,v",
            Bpo::value<bool>()->zero_tokens(),
            "print version string"
        )
    ;
    auto hidden = Bpo::options_description{"Options"};
    hidden.add_options()
        (
            "script-file",
            Bpo::value<std::string>(),
            "read script from file, use '-' to read script from stdin."
        )
    ;
    auto posOptions = Bpo::positional_options_description{};
    posOptions.add("script-file", 1);

    auto allOptions = Bpo::options_description{};
    allOptions.add(general).add(hidden);

    auto args = Bpo::variables_map{};
    Bpo::store(
        Bpo::command_line_parser(argn, argv)
            .options(allOptions)
            .positional(posOptions)
            .run(),
        args
    );
    Bpo::notify(args);

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
