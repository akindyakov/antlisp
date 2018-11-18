cc_library(
    name = "cell",
    srcs = [
        "cell.cpp",
        "cell_type.cpp",
    ],
    hdrs = [
        "cell.h",
        "cell_impl.h",
        "cell_type.h",
    ],
    deps = [
        "//antlisp/lib/util:util",
    ],
    visibility = ["PUBLIC"],
)

cc_test(
    name = "ut",
    srcs = [
        "cell_ut.cpp"
    ],
    deps = [
        ":cell",
        "//antlisp/lib/test:util",
    ],
    size = "small",
    visibility = ["PUBLIC"],
)
