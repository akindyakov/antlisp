cc_library(
    name = "builtin",
    srcs = [
        "io.cpp",
        "math.cpp",
    ],
    hdrs = [
        "io.h",
        "math.h",
    ],
    deps = [
        "//antlisp/lib/function:function",
        "//antlisp/lib/cell_ext:ext",
        "//antlisp/lib/cell:cell",
        "//antlisp/lib/util:util",
    ],
    visibility = ["PUBLIC"],
)

cc_test(
    name = "math_div_ut",
    srcs = [
        "math_div_ut.cpp",
    ],
    deps = [
        ":builtin",
        "//antlisp/lib/test:util",
    ],
    size = "small",
    visibility = ["PUBLIC"],
)

cc_test(
    name = "math_eq_ut",
    srcs = [
        "math_eq_ut.cpp",
    ],
    deps = [
        ":builtin",
        "//antlisp/lib/test:util",
    ],
    size = "small",
    visibility = ["PUBLIC"],
)

cc_test(
    name = "math_less_ut",
    srcs = [
        "math_less_ut.cpp",
    ],
    deps = [
        ":builtin",
        "//antlisp/lib/test:util",
    ],
    size = "small",
    visibility = ["PUBLIC"],
)

cc_test(
    name = "math_multiplication_ut",
    srcs = [
        "math_multiplication_ut.cpp",
    ],
    deps = [
        ":builtin",
        "//antlisp/lib/test:util",
    ],
    size = "small",
    visibility = ["PUBLIC"],
)

cc_test(
    name = "math_sum_ut",
    srcs = [
        "math_sum_ut.cpp",
    ],
    deps = [
        ":builtin",
        "//antlisp/lib/test:util",
    ],
    size = "small",
    visibility = ["PUBLIC"],
)

cc_test(
    name = "math_ut",
    srcs = [
        "math_ut.cpp",
    ],
    deps = [
        ":builtin",
        "//antlisp/lib/test:util",
    ],
    size = "small",
    visibility = ["PUBLIC"],
)
