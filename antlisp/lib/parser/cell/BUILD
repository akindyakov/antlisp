cc_library(
    name = "cell_in_string",
    srcs = [
        "cell_in_string.cpp",
    ],
    hdrs = [
        "cell_in_string.h",
    ],
    deps = [
        "//antlisp/lib/cell:cell",
        "//antlisp/lib/cell_ext:ext",
        "//antlisp/lib/function:function",
        "//antlisp/lib/parser:parser_error",
        "//antlisp/lib/util:util",
    ],
    visibility = ["PUBLIC"],
)

cc_test(
    name = "ut",
    srcs = [
        "cell_in_string_ut.cpp",
    ],
    deps = [
        ":cell_in_string",
        "//antlisp/lib/test:util",
    ],
    size = "small",
    visibility = ["PUBLIC"],
)
