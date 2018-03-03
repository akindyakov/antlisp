#include "args.h"

#include <antlisp/lib/parser/parser.h>
#include <antlisp/lib/function/stack_machine.h>

#include <iostream>


AntLisp::Namespace interactive() {
    auto global = AntLisp::Namespace{};
    auto line = std::string{};
    while (std::cin) {
        std::cout << "> " << std::flush;
        std::getline(std::cin, line, '\n');
        //**/ std::cout << "(line \"" << line << "\")\n";
        std::istringstream in(line);
        auto env = AntLisp::Environment(
            AntLisp::parseCode(in, global)
        );
        env.run();
        std::cout << env.ret.toString() << std::endl;
        global = std::move(env.vars);
    }
    return global;
}

AntLisp::Namespace fromFile(
    const std::string& filename
) {
    // TODO
    std::cout << "There is a code file " << filename << "\n";
    return AntLisp::Namespace{};
}

AntLisp::Namespace fromString(
    const std::string& code
) {
    // TODO
    std::cout << "There is a code " << code << "\n";
    return AntLisp::Namespace{};
}

int main(int argn, char** argv) {
    auto argsOpt = Argparse(argn, argv);
    if (not argsOpt) {
        return 0;
    }
    const auto& args = argsOpt.get();

    if (args.count("version")) {
        std::cout << "version\n";
        return 0;
    }

    auto globalVars = AntLisp::Namespace{};
    if (args.count("script-file")) {
        globalVars = fromFile(
            args["script-file"].as<std::string>()
        );
    } else if (args.count("cmd")) {
        globalVars = fromString(
            args["cmd"].as<std::string>()
        );
    } else {
        globalVars = interactive();
    }
    if (args.count("print-global")) {
        for (const auto& nameAndCell : globalVars) {
            std::cout
                << "{"
                << nameAndCell.first << " " << nameAndCell.second.toString()
                << "}\n";
        }
    }
    return 0;
}
