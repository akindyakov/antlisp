cc_library(
    name = "string",
    srcs = [
        "string.cpp",
    ],
    hdrs = [
        "string.h",
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
        "string_ut.cpp",
    ],
    deps = [
        ":string",
        "//antlisp/lib/test:util",
    ],
    size = "small",
    visibility = ["PUBLIC"],
)

