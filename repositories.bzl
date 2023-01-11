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
        sha256 = "fe4b366bf928dad7a074037f98651a4cfc2f7d132e150e0542ae87ea7172dca9",
        strip_prefix = "bazel_sundry-c3735667ea3d31ff78a8398359f49781fe75b7aa",
        url = "https://github.com/seaube/bazel_sundry/archive/c3735667ea3d31ff78a8398359f49781fe75b7aa.zip",
    )

    http_archive(
        name = "ecsact_runtime",
        sha256 = "219ce161deeb4865d3c190eac300dd257671e2b2ca0ec8752091506ccfa554a2",
        strip_prefix = "ecsact_runtime-5cbf7257d7cbc42762d22e7604bc5bc0efb837fe",
        url = "https://github.com/ecsact-dev/ecsact_runtime/archive/5cbf7257d7cbc42762d22e7604bc5bc0efb837fe.zip",
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
