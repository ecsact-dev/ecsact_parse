load("@bazel_tools//tools/build_defs/repo:http.bzl", _http_archive = "http_archive")
load("@bazel_tools//tools/build_defs/repo:utils.bzl", "maybe")

_LEXY_BUILD_FILE_CONTENT = """
load("@rules_cc//cc:defs.bzl", "cc_library")

cc_library(
    name = "lexy",
    visibility = ["//visibility:public"],
    includes = ["include"],
    hdrs = glob(["include/**/*.hpp"]),
    srcs = ["src/input/file.cpp"],
    strip_include_prefix = "include",
    copts = select({
        "@bazel_tools//tools/cpp:msvc": ["/std:c++latest"],
        "//conditions:default": ["-std=c++20"],
    }),
)
"""

def http_archive(name, **kwargs):
    maybe(_http_archive, name = name, **kwargs)

def ecsact_parse_dependencies():
    http_archive(
        name = "bazel_sundry",
        sha256 = "4ee3c8e29eb3d4f34b1ca7d2374437b5c6f0fd62316f90e38e9f32119421466d",
        strip_prefix = "bazel_sundry-de785e9f424948949cafed3ee8bee9689dc84a80",
        url = "https://github.com/seaube/bazel_sundry/archive/de785e9f424948949cafed3ee8bee9689dc84a80.zip",
    )

    http_archive(
        name = "ecsact_runtime",
        sha256 = "1d6c151c0f0322bfffccb4c45c1a2d8928c372903a4084b26f9d9d945fd32daf",
        strip_prefix = "ecsact_runtime-ca50160ae21d14fabe25c4f6b6fc8a569f7e5271",
        url = "https://github.com/ecsact-dev/ecsact_runtime/archive/ca50160ae21d14fabe25c4f6b6fc8a569f7e5271.zip",
    )

    http_archive(
        name = "lexy",
        build_file_content = _LEXY_BUILD_FILE_CONTENT,
        sha256 = "de2199f8233ea5ed9d4dbe86a8eaf88d754decd28e28554329a7b29b4d952773",
        url = "https://github.com/foonathan/lexy/releases/download/v2022.05.1/lexy-src.zip",
    )

    http_archive(
        name = "magic_enum",
        sha256 = "6b948d1680f02542d651fc82154a9e136b341ce55c5bf300736b157e23f9df11",
        strip_prefix = "magic_enum-0.8.1",
        url = "https://github.com/Neargye/magic_enum/archive/refs/tags/v0.8.1.tar.gz",
    )
