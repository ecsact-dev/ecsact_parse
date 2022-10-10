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
        sha256 = "be8c54735b3db6daac5998abafbff35393802a90c18db8dfb0afe0d1ce404d7e",
        strip_prefix = "ecsact_runtime-93ef909e9198e6607c10687a0880fa82f1a95f30",
        url = "https://github.com/ecsact-dev/ecsact_runtime/archive/93ef909e9198e6607c10687a0880fa82f1a95f30.zip",
    )

    http_archive(
        name = "lexy",
        build_file_content = _LEXY_BUILD_FILE_CONTENT,
        sha256 = "59607e4e2691c0c03068f1f9a7043f2d4703f5c9c48ec42a318b43aae9468386",
        url = "https://github.com/foonathan/lexy/releases/download/v2022.05.1/lexy-src.zip",
    )

    http_archive(
        name = "magic_enum",
        sha256 = "6b948d1680f02542d651fc82154a9e136b341ce55c5bf300736b157e23f9df11",
        strip_prefix = "magic_enum-0.8.1",
        url = "https://github.com/Neargye/magic_enum/archive/refs/tags/v0.8.1.tar.gz",
    )
