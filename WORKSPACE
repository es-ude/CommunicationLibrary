workspace(
    name = "CommunicationLibrary",
)

load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")

http_archive(
    name = "EmbeddedSystemsBuildScripts",
    strip_prefix = "EmbeddedSystemsBuildScripts-1.0.1",
    type = "tar.gz",
    urls = ["https://github.com/es-ude/EmbeddedSystemsBuildScripts/archive/v1.0.1.tar.gz"],
)

load("@EmbeddedSystemsBuildScripts//Toolchains/Avr:avr.bzl", "avr_toolchain")

avr_toolchain()

http_archive(
    name = "Unity",
    build_file = "@EmbeddedSystemsBuildScripts//:BUILD.Unity",
    strip_prefix = "Unity-master",
    urls = ["https://github.com/ThrowTheSwitch/Unity/archive/master.tar.gz"],
    # sha256 = "2757ff718ef4c507a7c294f62bfd8d63a780b9122840c9b28ca376784f3baa6e",
)

http_archive(
    name = "CException",
    build_file = "@EmbeddedSystemsBuildScripts//:BUILD.CException",
    strip_prefix = "CException-master",
    urls = ["https://github.com/ThrowTheSwitch/CException/archive/master.tar.gz"],
    # sha256 = "f5af035d949445e5b74fd0505a8ef1413109fcfd31d8ee7333516daad23483f7",
)

http_archive(
    name = "CMock",
    build_file = "@EmbeddedSystemsBuildScripts//:BUILD.CMock",
    strip_prefix = "CMock-master",
    urls = ["https://github.com/ThrowTheSwitch/CMock/archive/master.tar.gz"],
    # sha256 = "89f4a546b07ff03eac42563b2228edccd1dbfc1e9600581330826e239c54a225",
)

http_archive(
    name = "LUFA",
    build_file = "@EmbeddedSystemsBuildScripts//:BUILD.LUFA",
    sha256 = "44294343284717ca63f07e7788680410a994a3c7f8ce27a784783bbfee09430e",
    strip_prefix = "lufa-LUFA-170418",
    urls = ["http://fourwalledcubicle.com/files/LUFA/LUFA-170418.zip"],
)

http_archive(
    name = "EmbeddedUtilities",
    sha256 = "49e7d2c02bf5bcc10afe94f1d8233f2f0601c7a166d38e32409e2312abf7950e",
    strip_prefix = "EmbeddedUtilities-0.3.2",
    urls = ["https://github.com/es-ude/EmbeddedUtilities/archive/v0.3.2.tar.gz"],
)

http_archive(
    name = "PeripheralInterface",
    sha256 = "4f00ab56c6a7485ad813423c156cc6b895a68d3625a5c8e534a3be38aed5337d",
    strip_prefix = "PeripheralInterface-0.7.1",
    urls = ["https://github.com/es-ude/PeripheralInterface/archive/v0.7.1.tar.gz"],
)
