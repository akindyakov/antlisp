cc_library(
    name = "parser_error",
    hdrs = [
        "error.h",
    ],
    deps = [
        "//antlisp/lib/util:util",
    ],
    visibility = ["PUBLIC"],
)

cc_library(
    name = "parser",
    srcs = [
        "parser.cpp",
    ],
    hdrs = [
        "parser.h",
        "parser_impl.h",
    ],
    deps = [
        "//antlisp/lib/builtin:builtin",
        "//antlisp/lib/function:function",
        "//antlisp/lib/parser/cell:cell_in_string",
        "//antlisp/lib/parser/parentheses",
        "//antlisp/lib/util:util",
        ":parser_error",
    ],
    visibility = ["PUBLIC"],
)

cc_test(
    name = "parse_and_run_ut",
    srcs = [
        "parse_and_run_ut.cpp",
    ],
    deps = [
        ":parser",
        "//antlisp/lib/test:util",
    ],
    size = "small",
    visibility = ["PUBLIC"],
)

cc_test(
    name = "parser_ut",
    srcs = [
        "parser_ut.cpp",
    ],
    deps = [
        ":parser",
        "//antlisp/lib/test:util",
    ],
    size = "small",
    visibility = ["PUBLIC"],
)
