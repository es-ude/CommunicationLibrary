load(":doc/docs.bzl", "doxygen_archive", "sphinx_archive")

doxygen_archive(
    name = "doxy",
    srcs = [":CommunicationModuleIncl"],
    doxyfile = ":doc/doxy.conf",
)

sphinx_archive(
    name = "sphinx",
    srcs = [
        "CommunicationModule/Readme.rst",
        "Readme.rst",
    ],
    config_file = ":doc/conf.py",
    doxygen_xml_archive = ":doxy",
)

filegroup(
    name = "CommunicationModuleSrc",
    srcs = glob(
        [
            "src/**/*.c",
            "src/**/*.h",
        ],
    ),
)

filegroup(
    name = "CommunicationModuleIncl",
    srcs = [
        "CommunicationModule/CommunicationModule.h",
        "CommunicationModule/FrameHeader802154Struct.h",
        "CommunicationModule/Mac802154.h",
        "CommunicationModule/Mac802154MRFImpl.h",
    ],
)

exports_files(
    glob([
        "src/**/*.h",
        "src/**/*.c",
    ]),
    visibility = [
        "//test:__pkg__",
        "//test:__subpackages__",
        "@CMock//CMock:__pkg__",
    ],
)

exports_files(
    glob([
        "CommunicationModule/*.h",
    ]),
    visibility = [
        "//visibility:public",
    ],
)

cc_library(
    name = "CommunicationModule",
    srcs = [":CommunicationModuleSrc"],
    hdrs = [":CommunicationModuleIncl"],
    copts = ["-DDEBUG=0"],
    visibility = ["//visibility:public"],
    deps = [
        "@EmbeddedUtilities//:BitManipulation",
        "@EmbeddedUtilities//:Debug",
        "@PeripheralInterface",
    ],
)

cc_library(
    name = "CommunicationModuleHdrOnly",
    hdrs = [":CommunicationModuleIncl"],
    linkstatic = True,
    visibility = ["//visibility:public"],
    deps = [
        "@PeripheralInterface//:PeripheralInterfaceHdrsOnly",
    ],
)

#########################################
### Packaging for Artifactory         ###
#########################################

load("@bazel_tools//tools/build_defs/pkg:pkg.bzl", "pkg_tar")

pkg_tar(
    name = "pkg",
    srcs = [
        ":BUILD",
        ":CommunicationModuleIncl",
        ":CommunicationModuleSrc",
    ],
    extension = "tar.gz",
    mode = "0644",
    strip_prefix = ".",
    deps = [
        "//Setup:pkg",
        "//configs:pkg",
        "//constraints:pkg",
        "//platforms:pkg",
    ],
)
