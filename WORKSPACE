workspace(
    name = "CommunicationLibrary",
)

load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")
load("@bazel_tools//tools/build_defs/repo:git.bzl", "git_repository")

http_archive(
    name = "EmbeddedSystemsBuildScripts",
    strip_prefix = "EmbeddedSystemsBuildScripts-0.5.3",
    type = "tar.gz",
    urls = ["https://github.com/es-ude/EmbeddedSystemsBuildScripts/archive/v0.5.3.tar.gz"],
    sha256 = "b06d3fd0065e70fcad15a2e81adfcb86a8d53c4fb44c44b5e02b8fd2b73e8e5a",
)

load("@EmbeddedSystemsBuildScripts//AvrToolchain:avr.bzl", "avr_toolchain")

avr_toolchain()

http_archive(
    name = "Unity",
    build_file = "@EmbeddedSystemsBuildScripts//:BUILD.Unity",
    strip_prefix = "Unity-master",
    urls = ["https://github.com/ThrowTheSwitch/Unity/archive/master.tar.gz"],
    sha256 = "2757ff718ef4c507a7c294f62bfd8d63a780b9122840c9b28ca376784f3baa6e",
)

http_archive(
    name = "CException",
    build_file = "@EmbeddedSystemsBuildScripts//:BUILD.CException",
    strip_prefix = "CException-master",
    urls = ["https://github.com/ThrowTheSwitch/CException/archive/master.tar.gz"],
    sha256 = "f5af035d949445e5b74fd0505a8ef1413109fcfd31d8ee7333516daad23483f7",
)

http_archive(
    name = "CMock",
    build_file = "@EmbeddedSystemsBuildScripts//:BUILD.CMock",
    strip_prefix = "CMock-master",
    urls = ["https://github.com/ThrowTheSwitch/CMock/archive/master.tar.gz"],
    sha256 = "89f4a546b07ff03eac42563b2228edccd1dbfc1e9600581330826e239c54a225",
)

http_archive(
    name = "LUFA",
    build_file = "@EmbeddedSystemsBuildScripts//:BUILD.LUFA",
    strip_prefix = "lufa-LUFA-170418",
    urls = ["http://fourwalledcubicle.com/files/LUFA/LUFA-170418.zip"],
    sha256 = "44294343284717ca63f07e7788680410a994a3c7f8ce27a784783bbfee09430e",
)

http_archive(
    name = "EmbeddedUtilities",
    strip_prefix = "EmbeddedUtil-0.3",
    urls = ["https://github.com/es-ude/EmbeddedUtil/archive/v0.3.tar.gz"],
    sha256 = "964eb7b1ab9afba6960a6d19f7733716f61ba588c82a19330d87e4f006fce941",
)

http_archive(
    name = "PeripheralInterface",
    strip_prefix = "PeripheralInterface-0.7",
    urls = ["https://github.com/es-ude/PeripheralInterface/archive/v0.7.tar.gz"],
    sha256 = "03aa03c8897b66227ddd1ddf6793a59f6b375b41f0f2329506f49d5ca4a9130a",
)
