load("@AVR_Toolchain//:helpers.bzl", "default_embedded_lib")

filegroup(
    name = "CommunicationModuleSrcLocal",
    srcs = glob(
        [
            "src/**/*.c",
            "src/**/*.h",
        ],
        exclude = ["src/Util/Mutex.c"],
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
        "CommunicationModule/Mac802154.h",
        "CommunicationModule/Mac802154MRFImpl.h",
        "CommunicationModule/FrameHeader802154Struct.h",
    ] + glob([
        "Peripheral/*.h",
        "Debug/*.h",
    ]),
)

exports_files(
    glob([
        "Peripheral/**/*.h",
        "Debug/*.h",
        "CommunicationModule/*.h",
        "src/**/*.h",
        "src/**/*.c",
        "Util/**/*.h",
    ]),
    visibility = [
        "//test:__pkg__",
        "//test:__subpackages__",
        "@CMock//CMock:__pkg__",
    ],
)

default_embedded_lib(
    name = "CommunicationModule",
    srcs = select({
        "@AVR_Toolchain//:avr-config": [":CommunicationModuleSrc"],
        "//conditions:default": [":CommunicationModuleSrcLocal"],
    }),
    hdrs = [":CommunicationModuleIncl"],
    visibility = ["//visibility:public"],
    deps = [
        "@Util//:BitManipulation",
        "@PeripheralInterface"
    ],
)

filegroup(
    name = "PeripheralInterfaceHdrs",
    srcs = glob(["Peripheral/*.h"]),
)

filegroup(
    name = "PeripheralInterfaceSrcs",
    srcs = glob([
        "src/Peripheral/*.h",
        "src/Peripheral/*.c",
    ]),
)

default_embedded_lib(
    name = "EmbeddedUtil",
    srcs = ["src/Util/Mutex.c"],
    hdrs = [
        "Util/Atomic.h",
        "Util/Callback.h",
        "Util/Mutex.h",
        "src/Util/BitManipulation.h",
    ],
    visibility = ["//visibility:public"],
)

default_embedded_lib(
    name = "PeripheralInterface",
    srcs = [":PeripheralInterfaceSrcs"],
    hdrs = [":PeripheralInterfaceHdrs"],
    visibility = ["//visibility:public"],
    deps = [
        ":EmbeddedUtil",
    ],
)

default_embedded_lib(
    name = "Debug",
    srcs = [
        "src/Debug/Debug.c",
    ],
    hdrs = ["Debug/Debug.h"],
    deps = [
        "//:PeripheralInterface",
    ],
)
#########################################
### Generate zip file for publishing  ###
#########################################

genzip_cmd = "zip -j $(OUTS) $(locations :CommunicationModule); zip $(OUTS) $(locations :CommunicationModuleIncl)"

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
