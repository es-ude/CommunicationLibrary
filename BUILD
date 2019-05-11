filegroup(
    name = "CommunicationModuleSrcLocal",
    srcs = glob(
        [
            "src/**/*.c",
            "src/**/*.h",
        ],
    ),
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
        "CommunicationModule/*.h"
    ]),
    visibility = [
        "//visibility:public",
    ]
)

cc_library(
    name = "CommunicationModule",
    srcs = [":CommunicationModuleSrc"],
    hdrs = [":CommunicationModuleIncl"],
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
    visibility = ["//visibility:public"],
    deps = [
        "@PeripheralInterface//:PeripheralInterfaceHdrsOnly",
    ],
    linkstatic = True,
)
#########################################
### Packaging for Artifactory         ###
#########################################

load("@bazel_tools//tools/build_defs/pkg:pkg.bzl", "pkg_tar")

pkg_tar(
    name = "HeadersTar",
    srcs = [":CommunicationModuleIncl"],
    extension = "tar.gz",
    mode = "0644",
    package_dir = "/CommunicationModule",
)

pkg_tar(
    name = "LibArchiveTar",
    srcs = [":CommunicationModule"],
    extension = "tar.gz",
    mode = "0644",
    package_dir = "/lib",
    strip_prefix = "/",
)

pkg_tar(
    name = "CommunicationModule-bin",
    extension = "tar.gz",
    mode = "0644",
    deps = [
        ":BuildFile",
        ":HeadersTar",
        ":LibArchiveTar",
    ],
)

pkg_tar(
    name = "BuildFile",
    srcs = ["BUILD.Artifactory.tpl"],
    extension = "tar.gz",
    mode = "0644",
    remap_paths = {
        "BUILD.ArtifactoryCommLib.tpl": "BUILD",
    },
)

pkg_tar(
    name = "CommunicationModuleSetupBUILD",
    srcs = ["BUILD.PredefinedSetup.tpl"],
    extension = "tar.gz",
    mode = "0644",
    remap_paths = {
        "BUILD.PredefinedSetup.tpl": "BUILD",
    },
)

pkg_tar(
    name = "CommunicationModuleSetupArchive",
    srcs = select({
        "@AvrToolchain//config:Motherboard": ["//integration_tests:MotherboardSetup"],
        "@AvrToolchain//config:ElasticNode": ["//integration_tests:ElasticNodeSetup"],
    }),
    extension = "tar.gz",
    mode = "0644",
    package_dir = "lib",
    remap_paths = {
        "integration_tests/libElasticNodeSetup.lo": "libHardwareSetup.lo",
        "integration_tests/libMotherboardSetup.lo": "libHardwareSetup.lo",
    },
    # without the strip_prefix=".", the directory order is not preserved.
    # see https://groups.google.com/forum/#!topic/bazel-discuss/LHjopuUvZks for more info
    strip_prefix = ".",
)

pkg_tar(
    name = "CommunicationModuleSetupHdrs",
    srcs = ["//integration_tests:SetupHdrs"],
    extension = "tar.gz",
    mode = "0644",
    package_dir = "Setup/",
)

pkg_tar(
    name = "CommunicationModuleSetup",
    extension = "tar.gz",
    mode = "0644",
    deps = [
        ":CommunicationModuleSetupArchive",
        ":CommunicationModuleSetupBUILD",
        ":CommunicationModuleSetupHdrs",
    ],
)
