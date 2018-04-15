#!/usr/bin/env python
# encoding: utf-8

import os


top = "."
out = "build-waf"


def options(opt):
    opt.load([
        "compiler_cxx",
        "waf_unit_test",
    ])


def configure(ctx):
    ctx.load([
        "compiler_cxx",
        "waf_unit_test",
    ])
    ctx.env.append_unique(
        "CXXFLAGS", [
            "-O2",
            "-std=c++14",
            "-ggdb",
            "-Wall",
            "-Wextra",
        ]
    )
    ctx.env.INCLUDES = [
        os.path.abspath(top),
        os.path.abspath(out),
    ]


def build(ctx):
    ctx.recurse([
        "antlisp",
    ])
