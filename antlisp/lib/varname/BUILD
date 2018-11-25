cc_library(
    name = "varname",
    srcs = [
        "prefix.cpp",
        "namespace.cpp",
    ],
    hdrs = [
        "namespace.h",
        "prefix.h",
        "varname.h",
    ],
    deps = [
        "//antlisp/lib/cell:cell",
        "//antlisp/lib/util:util",
        "@boost//:boost",
    ],
    visibility = ["PUBLIC"],
)

cc_test(
    name = "prefix_ut",
    srcs = [
        "prefix_ut.cpp",
    ],
    deps = [
        ":varname",
        "//antlisp/lib/test:util",
    ],
    size = "small",
    visibility = ["PUBLIC"],
)

cc_test(
    name = "namespace_ut",
    srcs = [
        "namespace_ut.cpp",
    ],
    deps = [
        ":varname",
        "//antlisp/lib/test:util",
    ],
    size = "small",
    visibility = ["PUBLIC"],
)
