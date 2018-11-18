cc_library(
    name = "code_stream",
    srcs = [
        "code_stream.cpp",
    ],
    hdrs = [
        "code_stream.h",
    ],
    deps = [
        "//antlisp/lib/util:util",
    ],
    visibility = ["PUBLIC"],
)

cc_test(
    name = "ut",
    srcs = [
        "code_stream_ut.cpp",
    ],
    deps = [
        ":code_stream",
        "//antlisp/lib/test:util",
    ],
    size = "small",
    visibility = ["PUBLIC"],
)
