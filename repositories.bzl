load("@bazel_tools//tools/build_defs/repo:http.bzl", _http_archive = "http_archive")
load("@bazel_tools//tools/build_defs/repo:utils.bzl", "maybe")

def http_archive(name, **kwargs):
    maybe(_http_archive, name = name, **kwargs)

def ecsact_parse_dependencies():
    http_archive(
        name = "bazel_sundry",
        sha256 = "3e6a77b27b99aea693fed5b7fbca27d1ece8b5f1ab492e12f977d4c282a5d97b",
        strip_prefix = "bazel_sundry-cca04d615a173685469ef4d94a6473d0783fab8b",
        url = "https://github.com/seaube/bazel_sundry/archive/cca04d615a173685469ef4d94a6473d0783fab8b.zip",
    )

    http_archive(
        name = "ecsact_runtime",
        sha256 = "219ce161deeb4865d3c190eac300dd257671e2b2ca0ec8752091506ccfa554a2",
        strip_prefix = "ecsact_runtime-5cbf7257d7cbc42762d22e7604bc5bc0efb837fe",
        url = "https://github.com/ecsact-dev/ecsact_runtime/archive/5cbf7257d7cbc42762d22e7604bc5bc0efb837fe.zip",
    )
