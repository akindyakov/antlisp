#include "args.h"

#include <iostream>


int main(int argn, char** argv) {
    auto argsOpt = Argparse(argn, argv);
    if (not argsOpt) {
        return 0;
    }
    const auto& args = argsOpt.get();
    if (args.count("file")) {
        std::cout
            << "There is a code file " << args["file"].as<std::string>()
            << '\n';
    }
    return 0;
}
