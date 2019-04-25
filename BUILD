load("@AvrToolchain//:helpers.bzl", "default_embedded_lib")

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
        "CommunicationModule/*.h",
        "src/**/*.h",
        "src/**/*.c",
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
        "@AvrToolchain//:avr-config": [":CommunicationModuleSrc"],
        "//conditions:default": [":CommunicationModuleSrcLocal"],
    }),
    hdrs = [":CommunicationModuleIncl"],
    visibility = ["//visibility:public"],
    deps = [
        "@EmbeddedUtilities//:BitManipulation",
        "@EmbeddedUtilities//:Mutex",
        "@PeripheralInterface",
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
