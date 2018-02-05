#!/usr/bin/env python
# encoding: utf-8

import os


top = "."
out = "build-waf"


def options(opt):
    opt.load("compiler_cxx")


def configure(ctx):
    ctx.load("compiler_cxx")
    ctx.env.append_unique(
        "CXXFLAGS", [
            "-O2",
            "-std=c++14",
        ]
    )
    ctx.env.INCLUDES = [
        os.path.abspath(top),
        os.path.abspath(out),
    ]
    print ctx.env.INCLUDES


def build(ctx):
    print ctx.env.INCLUDES
    ctx.recurse([
        "antlisp",
    ])
