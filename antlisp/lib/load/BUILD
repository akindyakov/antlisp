cc_library(
    name = "load",
    srcs = [
        "load.cpp",
    ],
    hdrs = [
        "load.h",
    ],
    deps = [
        "//antlisp/lib/builtin:builtin",
        "//antlisp/lib/function:function",
        "//antlisp/lib/parser/cell:cell_in_string",
        "//antlisp/lib/parser/parentheses",
        "//antlisp/lib/util:util",
        "@boost//:boost",
    ],
    visibility = ["PUBLIC"],
)

cc_test(
    name = "load_ut",
    srcs = [
        "load_ut.cpp",
    ],
    deps = [
        ":load",
        "//antlisp/lib/test:util",
    ],
    size = "small",
    visibility = ["PUBLIC"],
)
