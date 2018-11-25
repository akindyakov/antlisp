#include "args.h"

#include <antlisp/lib/parser/parser.h>
#include <antlisp/lib/function/tape_machine.h>
#include <antlisp/lib/builtin/math.h>
#include <antlisp/lib/builtin/io.h>
#include <antlisp/lib/interface.h>
#include <antlisp/lib/util/exception.h>

#include <fstream>
#include <iostream>


void interactive(
    AntLisp::Namespace& global
) {
    auto line = std::string{};
    while (std::cin) {
        std::cout << "> " << std::flush;
        std::getline(std::cin, line, '\n');
        std::istringstream in(line);
        try {
            auto machine = AntLisp::TapeMachine(
                AntLisp::parseCode(in, std::move(global))
            );
            machine.run();
            std::cout << machine.ret().toString() << '\n';
            global = machine.takeVars();
        }
        catch (AntLisp::Exception const& ex) {
            std::cout << "Error: " << ex.what() << '\n';
        }
    }
}

void fromStream(
    AntLisp::Namespace& global
    , std::istream& in
) {
    auto machine = AntLisp::TapeMachine(
        AntLisp::parseCode(in, std::move(global))
    );
    machine.run();
    global = machine.takeVars();
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
    AntLisp::NamesLoader::inject(global);
    AntLisp::Builtin::allMathFunctions(global);
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
        std::cout << global.toString() << '\n';
    }
    return 0;
}
