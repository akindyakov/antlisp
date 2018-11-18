cc_library(
    name = "error",
    srcs = [
        "error.cpp",
    ],
    hdrs = [
        "error.h",
    ],
    deps = [
        "//antlisp/lib/cell:cell",
        "//antlisp/lib/util:util",
    ],
    visibility = ["PUBLIC"],
)

cc_test(
    name = "ut",
    srcs = [
        "error_ut.cpp",
    ],
    deps = [
        ":error",
        "//antlisp/lib/test:util",
    ],
    size = "small",
    visibility = ["PUBLIC"],
)

