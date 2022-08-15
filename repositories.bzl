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
        sha256 = "c02c7400335f1e76b631c3aaa5ed6d9232e9290670bdacb6eac6166568ffe016",
        strip_prefix = "ecsact_runtime-37d42e018055b8856bc2190a678b4408131d7172",
        url = "https://github.com/ecsact-dev/ecsact_runtime/archive/37d42e018055b8856bc2190a678b4408131d7172.zip",
    )

    http_archive(
        name = "lexy",
        build_file_content = _LEXY_BUILD_FILE_CONTENT,
        sha256 = "59607e4e2691c0c03068f1f9a7043f2d4703f5c9c48ec42a318b43aae9468386",
        url = "https://github.com/foonathan/lexy/releases/download/v2022.05.1/lexy-src.zip",
    )

    http_archive(
        name = "magic_enum",
        sha256 = "5e7680e877dd4cf68d9d0c0e3c2a683b432a9ba84fc1993c4da3de70db894c3c",
        strip_prefix = "magic_enum-0.8.0",
        url = "https://github.com/Neargye/magic_enum/archive/refs/tags/v0.8.0.tar.gz",
    )
