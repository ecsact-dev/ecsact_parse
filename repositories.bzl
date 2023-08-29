load("@bazel_tools//tools/build_defs/repo:http.bzl", _http_archive = "http_archive")
load("@bazel_tools//tools/build_defs/repo:utils.bzl", "maybe")

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
        sha256 = "219ce161deeb4865d3c190eac300dd257671e2b2ca0ec8752091506ccfa554a2",
        strip_prefix = "ecsact_runtime-5cbf7257d7cbc42762d22e7604bc5bc0efb837fe",
        url = "https://github.com/ecsact-dev/ecsact_runtime/archive/5cbf7257d7cbc42762d22e7604bc5bc0efb837fe.zip",
    )
