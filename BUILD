# makes the configuration setting "avr-config" available as soon as the cpu value avr is used for a build
config_setting(
    name = "avr-config",
    values = {
        "cpu": "avr",
    },
)

filegroup(
    name = "CommunicationModuleSrc",
    srcs = glob(["src/Mac802154/**/*.c"]) + [
        "src/Util/BitManipulation.c",
    ],
)

filegroup(
    name = "PrivateHdrFiles",
    srcs = glob(["src/Mac802154/**/*.h"]) + [
        "src/Util/BitManipulation.h",
    ],
)

filegroup(
    name = "CommunicationModuleIncl",
    srcs = [
        "CommunicationModule/CommunicationModule.h",
        "CommunicationModule/Mac802154.h",
        "CommunicationModule/Mac802154MRFImpl.h",
    ],
)

exports_files(
    glob([
        "Peripheral/**/*.h",
        "Debug/*.h",
        "CommunicationModule/*.h",
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
    "-std=gnu99",
]

CommunicationModuleLinkerFlags = [
    "-Xlinker --gc-sections",
]

CommunicationModuleVisibility = [
    "//visibility:public",
]

CommunicationModuleDeps = [
    "@CException//:CException",
    "//:Debug",
]

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
    linkstatic = True,
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
            "-Winline",
            "-Xlinker --gc-sections",
            "-ffast-math",
        ],
        "//conditions:default": [],
    }) + CommunicationModuleCompilerFlags,
    linkopts = [
        "-Wl,-z,norelro",
    ],
    visibility = [
        "//integration_tests:__pkg__",
        "//test:__subpackages__",
    ],
    deps = [
        "//:Debug",
        "//:PeripheralInterface",
        "@CException",
    ],
)

cc_library(
    name = "PeripheralInterface",
    srcs = [
        "src/Peripheral/PeripheralInterface.c",
        "src/Peripheral/PeripheralIntern.h",
        "src/Peripheral/PeripheralSPIImpl.c",
        "src/Peripheral/PeripheralSPIImplIntern.h",
        "src/Peripheral/SpiPinNumbers.h",
        "src/Peripheral/Usart.c",
        "src/Peripheral/UsartIntern.h",
        "src/Util/BitManipulation.c",
        "src/Util/BitManipulation.h",
    ],
    hdrs = [
        "CommunicationModule/Callback.h",
        "CommunicationModule/Exception.h",
        "CommunicationModule/PeripheralSPIImpl.h",
        "CommunicationModule/Usart.h",
        "Peripheral/PeripheralInterface.h",
    ],
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
    deps = [
        "@CException",
    ],
)

cc_library(
    name = "Debug",
    srcs = [
        "src/Debug/Debug.c",
    ],
    hdrs = ["Debug/Debug.h"],
    visibility = CommunicationModuleVisibility,
    deps = [
        "//:PeripheralInterface",
    ],
)
#########################################
### Generate zip file for publishing  ###
#########################################

genzip_cmd = "zip -j $(OUTS) $(location :CommunicationModule); zip $(OUTS) $(locations :CommunicationModuleIncl)"

LibAndHeaderForPublishing = [
    ":CommunicationModule",
    ":CommunicationModuleIncl",
]

genrule(
    name = "CommunicationModuleZip",
    srcs = LibAndHeaderForPublishing,
    outs = ["CommunicationModule.zip"],
    cmd = genzip_cmd,
)
