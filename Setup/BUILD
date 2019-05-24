load(
    "@AvrToolchain//:helpers.bzl",
    "cpu_frequency_flag",
    "default_embedded_binaries",
    "default_embedded_binary",
    "mcu_avr_gcc_flag",
)

filegroup(
    name = "LufaSetup",
    srcs = [
        "LUFAHelpers.c",
        "LUFAHelpers.h",
    ],
)

filegroup(
    name = "SetupHdrs",
    srcs = [
        "DebugSetup.h",
        "HardwareSetup.h",
    ],
    visibility = ["//visibility:public"],
)

LUFA_COPTS = [
    "-Iexternal/LUFA/Demos/Device/ClassDriver/VirtualSerial/Config",
    "-isystem external/LUFA/",
]

cc_library(
    name = "DebugMotherBoard",
    srcs = [
        "DebugOutputMotherBoard.c",
        ":LufaSetup",
    ],
    hdrs = [
        "DebugSetup.h",
    ],
    copts = mcu_avr_gcc_flag() + cpu_frequency_flag() + LUFA_COPTS,
    deps = [
        "@EmbeddedUtilities//:Debug",
        "@LUFA//:VirtualSerial"
    ],
    alwayslink = True,
)

cc_library(
    name = "ElasticNodeSetup",
    srcs = [
        "Delay.c",
        "ElasticNodeHardwareSetup.c",
    ],
    hdrs = [
        "DebugSetup.h",
        "HardwareSetup.h",
    ],
    copts = mcu_avr_gcc_flag() + cpu_frequency_flag(),
    visibility = ["//visibility:public"],
    deps = [
        ":Atomic",
        "//:CommunicationModule",
        "@EmbeddedUtilities//:Debug",
    ],
)

cc_library(
    name = "Atomic",
    srcs = [
        "Atomic.c",
    ],
    copts = mcu_avr_gcc_flag() + cpu_frequency_flag(),
    visibility = ["//visibility:public"],
    deps = [
        "@EmbeddedUtilities//:Mutex",
    ],
    alwayslink = True,
)

cc_library(
    name = "MotherboardSetup",
    srcs = [
        "Delay.c",
        "MotherBoardHardwareSetup.c",
    ],
    hdrs = [
        "DebugSetup.h",
        "HardwareSetup.h",
    ],
    copts = mcu_avr_gcc_flag() + cpu_frequency_flag(),
    visibility = ["//visibility:public"],
    deps = [
        ":Atomic",
        ":DebugMotherBoard",
        "//:CommunicationModule",
    ],
)

load("@bazel_tools//tools/build_defs/pkg:pkg.bzl", "pkg_tar")

pkg_tar(
    name = "pkg",
    srcs = glob(["*"]),
    mode = "0644",
    extension = "tar.gz",
    package_dir = "Setup",
    visibility = ["//visibility:public"],
)