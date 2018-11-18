cc_library(
    name = "parentheses",
    srcs = [
        "parentheses_parser.cpp",
    ],
    hdrs = [
        "parentheses_parser.h",
    ],
    deps = [
        "//antlisp/lib/parser/parentheses/code_stream:code_stream",
        "//antlisp/lib/util:util",
    ],
    visibility = ["PUBLIC"],
)

cc_test(
    name = "ut",
    srcs = [
        "parentheses_parser_ut.cpp",
    ],
    deps = [
        ":parentheses",
        "//antlisp/lib/test:util",
    ],
    size = "small",
    visibility = ["PUBLIC"],
)
