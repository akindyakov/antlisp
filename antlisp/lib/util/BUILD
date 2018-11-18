cc_library(
    name="util",
    hdrs = [
        "exception.h",
        "optional.h",
        "string.h",
    ],
    srcs = [
        "string.cpp",
    ],
    visibility = ["PUBLIC"],
)

cc_test(
    name = "exception_ut",
    srcs = [
        "exception_ut.cpp",
    ],
    deps = [
        ":util",
        "//antlisp/lib/test:util",
    ],
    size = "small",
    visibility = ["PUBLIC"],
)

cc_test(
    name = "string_ut",
    srcs = [
        "string_ut.cpp",
    ],
    deps = [
        ":util",
        "//antlisp/lib/test:util",
    ],
    size = "small",
    visibility = ["PUBLIC"],
)
