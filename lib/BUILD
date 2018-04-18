# makes the configuration setting "avr-config" available as soon as the cpu value avr is used for a build
config_setting(
    name = "avr-config",
    values = {
        "cpu": "avr",
    },
)

filegroup(
    name = "CommunicationModuleSrc",
    srcs = glob([
        "src/*.c",
        "src/*.h",
    ]),
)

filegroup(
    name = "CommunicationModuleIncl",
    srcs = glob(["include/*.h"]),
)

filegroup(
    name = "PlatformDependencies",
    srcs = glob(["include/platform/**/*.h"]),
)

filegroup(
    name = "SPILayerIncl",
    srcs = glob(["include/Peripheral/*.h"]),
)

filegroup(
    name = "SPILayerSrc",
    srcs = glob(["src/Peripheral/*.c"]),
)

filegroup(
    name = "CommunicationLayerIncl",
    srcs = glob(["include/communicationLayer/*.h"]),
)

filegroup(
    name = "TransferLayerIncl",
    srcs = glob(["include/TransferLayer/*.h"]),
)
filegroup(
    name = "TransferLayerSrc",
    srcs = glob(["src/TransferLayer/*.c"]),
)

filegroup(
    name = "CommunicationLayerSrc",
    srcs = glob(["src/communicationLayer/*.c"]),
)

filegroup(
    name = "DepracatedHeaderFiles",
    srcs = glob(["include/DEPRACATED/*.h"]),
)



"""
The select function allows to make the right hand option of a attribute
depend on a command line argument. Attributes that allow this behaviour are called
configurable attributes. Here we refer to the avr-config setting from above.
This adds the command line flag -mmcu=$(MCU) as soon as the user calls Bazel like
    bazel <command> --cpu=avr --define MCU=<mcu-name>
and $(MCU) is replaced by <mcu-name>
"""

cc_library(
    name = "CommunicationLibrary",
    srcs = [":CommunicationModuleSrc",":SPILayerSrc", ":CommunicationLayerSrc",":TransferLayerSrc"],
    hdrs = [":CommunicationModuleIncl", ":PlatformDependencies", ":SPILayerIncl", ":CommunicationLayerIncl", ":DepracatedHeaderFiles", ":TransferLayerIncl"],
    copts = select({
        ":avr-config": ["-mmcu=$(MCU)"],
        "//conditions:default": [],
        }),
    visibility = [
        "//visibility:public"
        ],
    deps = ["@CException//:CException"],
    )
