workspace(
    name = "CommunicationLibrary",
)

load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")
load("@bazel_tools//tools/build_defs/repo:git.bzl", "git_repository")

http_archive(
    name = "EmbeddedSystemsBuildScripts",
    strip_prefix = "EmbeddedSystemsBuildScripts-0.5",
    type = "tar.gz",
    urls = ["https://github.com/es-uni-due/EmbeddedSystemsBuildScripts/archive/v0.5.tar.gz"],
)

load("@EmbeddedSystemsBuildScripts//AvrToolchain:avr.bzl", "avr_toolchain")

avr_toolchain()

http_archive(
    name = "Unity",
    build_file = "@EmbeddedSystemsBuildScripts//:BUILD.Unity",
    strip_prefix = "Unity-master",
    urls = ["https://github.com/ThrowTheSwitch/Unity/archive/master.tar.gz"],
)

http_archive(
    name = "CException",
    build_file = "@EmbeddedSystemsBuildScripts//:BUILD.CException",
    strip_prefix = "CException-master",
    urls = ["https://github.com/ThrowTheSwitch/CException/archive/master.tar.gz"],
)

http_archive(
    name = "CMock",
    build_file = "@EmbeddedSystemsBuildScripts//:BUILD.CMock",
    strip_prefix = "CMock-master",
    urls = ["https://github.com/ThrowTheSwitch/CMock/archive/master.tar.gz"],
)

http_archive(
    name = "LUFA",
    build_file = "@EmbeddedSystemsBuildScripts//:BUILD.LUFA",
    strip_prefix = "lufa-LUFA-170418",
    urls = ["http://fourwalledcubicle.com/files/LUFA/LUFA-170418.zip"],
)

http_archive(
    name = "EmbeddedUtilities",
    strip_prefix = "EmbeddedUtil-0.3",
    type = "tar.gz",
    urls = ["https://github.com/es-uni-due/EmbeddedUtil/archive/v0.3.tar.gz"],
)

http_archive(
    name = "PeripheralInterface",
    strip_prefix = "PeripheralInterface-0.6",
    urls = ["https://github.com/es-uni-due/PeripheralInterface/archive/v0.6.tar.gz"],
)
