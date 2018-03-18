#include "args.h"

#include <antlisp/lib/parser/parser.h>
#include <antlisp/lib/function/tape_machine.h>
#include <antlisp/lib/function/builtin/math.h>
#include <antlisp/lib/function/builtin/io.h>

#include <fstream>
#include <iostream>


void interactive(
    AntLisp::Namespace& global
) {
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
}

void fromStream(
    AntLisp::Namespace& global
    , std::istream& in
) {
    auto env = AntLisp::Environment(
        AntLisp::parseCode(in, global)
    );
    env.run();
    global = std::move(env.vars);
}

void fromFile(
    AntLisp::Namespace& global
    , const std::string& filename
) {
    std::ifstream in(filename);
    fromStream(global, in);
}

void fromString(
    AntLisp::Namespace& global
    , const std::string& code
) {
    std::istringstream in(code);
    fromStream(global, in);
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

    auto global = AntLisp::Namespace{};
    AntLisp::Builtin::allMathFunctions(global);
    AntLisp::Builtin::IO::all(global);
    if (args.count("script-file")) {
        fromFile(
            global,
            args["script-file"].as<std::string>()
        );
    } else if (args.count("cmd")) {
        fromString(
            global,
            args["cmd"].as<std::string>()
        );
    } else {
        interactive(global);
    }
    if (args.count("print-global")) {
        for (const auto& nameAndCell : global) {
            std::cout
                << "{"
                << nameAndCell.first << " " << nameAndCell.second.toString()
                << "}\n";
        }
    }
    return 0;
}
