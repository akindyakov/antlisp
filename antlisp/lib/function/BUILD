cc_library(
    name = "function",
    srcs = [
        "tape_machine.cpp",
        "interface.cpp",
        "native_tape.cpp",
    ],
    hdrs = [
        "interface.h",
        "native_tape.h",
        "tape_machine.h",
        "type_vocabulary.h",
    ],
    deps = [
        "//antlisp/lib/varname:varname",
        "//antlisp/lib/cell:cell",
        "//antlisp/lib/parser/parentheses/code_stream:code_stream",
        "//antlisp/lib/util:util",
    ],
    visibility = ["PUBLIC"],
)

cc_test(
    name = "tape_machine_ut",
    srcs = [
        "tape_machine_ut.cpp",
    ],
    deps = [
        ":function",
        "//antlisp/lib/test:util",
    ],
    size = "small",
    visibility = ["PUBLIC"],
)

cc_test(
    name = "native_tape_ut",
    srcs = [
        "native_tape_ut.cpp",
    ],
    deps = [
        ":function",
        "//antlisp/lib/test:util",
    ],
    size = "small",
    visibility = ["PUBLIC"],
)
