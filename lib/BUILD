# makes the configuration setting "avr-config" available as soon as the cpu value avr is used for a build
config_setting(
    name = "avr-config",
    values = {
        "cpu": "avr",
    },
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
    name = "PlatformSrc",
    srcs = glob(["src/platform/**/*.c"]),
)

filegroup(
    name = "UartIncl",
    srcs = glob(["include/usart/*.h"]),
)

filegroup(
    name = "UartSrc",
    srcs = glob(["src/usart/*.c"]),
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
    srcs = [
        ":PlatformSrc",
        ":TransferLayerSrc",
        ":UartSrc",
    ],
    hdrs = [
        ":CommunicationModuleIncl",
        ":PlatformDependencies",
        ":TransferLayerIncl",
        ":UartIncl",
    ],
    copts = select({
        ":avr-config": ["-mmcu=$(MCU)"],
        "//conditions:default": [],
    }),
    visibility = [
        "//visibility:public",
    ],
    deps = ["@CException//:CException"],
)
