# makes the configuration setting "avr-config" available as soon as the cpu value avr is used for a build
config_setting(
    name = "avr-config",
    values = {
        "cpu": "avr",
    },
)

filegroup(
    name = "CommunicationModuleSrc",
    srcs = glob(
        [
            "src/**/*.c",
        ],
        exclude = ["src/platform/dummy/io.c"],
    ),
)

filegroup(
    name = "PrivateHdrFiles",
    srcs = glob(["src/**/*.h"]),
)

filegroup(
    name = "CommunicationModuleSrcForLocalBuild",
    srcs = [":CommunicationModuleSrc"] + ["src/platform/dummy/io.c"],
)

filegroup(
    name = "CommunicationModuleIncl",
    srcs = glob(["include/**/*.h"]),
)

exports_files(
    glob([
        "include/**/*.h",
        "src/**/*.h",
    ]),
    visibility = [
        "//test:__pkg__",
        "//test:__subpackages__",
        "@CMock//CMock:__pkg__",
    ],
)

"""
The select function allows to make the right hand option of a attribute
depend on a command line argument. Attributes that allow this behaviour are called
configurable attributes. Here we refer to the avr-config setting from above.
This adds the command line flag -mmcu=$(MCU) as soon as the user calls Bazel like
    bazel <command> --cpu=avr --define MCU=<mcu-name>
and $(MCU) is replaced by <mcu-name>
"""

CommunicationModuleCompilerFlags = [
    "-Winline",
    "-include stdint.h",
    "-DCEXCEPTION_T=uint8_t",
    "-DCEXCEPTION_NONE=0x00",
    "-funsigned-char",
    "-ffunction-sections",
    "-fdata-sections",
    "-fshort-enums",
    "-ffast-math",
    "-Os",
]

CommunicationModuleLinkerFlags = [
    "-Xlinker --gc-sections",
]

CommunicationModuleVisibility = [
    "//visibility:public",
]

CommunicationModuleDeps = [
    "@CException//:CException",
]

cc_library(
    name = "CommunicationModuleAtmega32u4",
    srcs = [
        ":CommunicationModuleSrc",
        ":PrivateHdrFiles",
    ],
    hdrs = [":CommunicationModuleIncl"],
    copts = [
        "-mmcu=atmega32u4",
    ] + CommunicationModuleCompilerFlags,
    linkopts = CommunicationModuleLinkerFlags,
    visibility = CommunicationModuleVisibility,
    deps = CommunicationModuleDeps,
)

cc_library(
    name = "CommunicationModule",
    srcs = [
        ":CommunicationModuleSrc",
        ":PrivateHdrFiles",
    ],
    hdrs = [":CommunicationModuleIncl"],
    copts = select({
        ":avr-config": [
            "-mmcu=$(MCU)",
            "-Os",
            "-std=c99",
        ],
        "//conditions:default": [],
    }) + CommunicationModuleCompilerFlags,
    linkopts = CommunicationModuleLinkerFlags,
    visibility = CommunicationModuleVisibility,
    deps = CommunicationModuleDeps,
)

cc_library(
    name = "CommunicationModuleLibForTest",
    srcs = [":CommunicationModuleSrc"],
    hdrs = [
        ":CommunicationModuleIncl",
        ":PrivateHdrFiles",
    ],
    copts = select({
        ":avr-config": [
            "-mmcu=$(MCU)",
            "-Os",
            "-fshort-enums",
            "-x c",
            "-funsigned-char",
            "-ffunction-sections",
            "-fdata-sections",
            "-Xlinker --gc-sections",
            "-ffast-math",
        ],
        "//conditions:default": [],
    }) + [
        "-Winline",
        "-include stdint.h",
        "-DCEXCEPTION_T=uint8_t",
        "-DCEXCEPTION_NONE=0x00",
                    "-std=c99",

    ],
    linkopts = [
        "-Wl,-z,norelro",
    ],
    visibility = [
        "//integration_tests:__pkg__",
        "//test:__subpackages__",
    ],
    deps = ["@CException//:CException"],
)
