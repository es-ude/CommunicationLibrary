load("@AvrToolchain//:helpers.bzl", "default_embedded_binaries", "default_embedded_binary")

LUFA_COPTS = [
    "-Iexternal/LUFA/Demos/Device/ClassDriver/VirtualSerial/Config",
    "-pipe",
    "-gdwarf-2",
    "-g2",
    "-fshort-enums",
    "-fno-inline-small-functions",
    "-fpack-struct",
    "-Wall",
    "-fno-strict-aliasing",
    "-funsigned-char",
    "-funsigned-bitfields",
    "-ffunction-sections",
    "-DARCH=ARCH_AVR8",
    "-DF_CPU=8000000UL",
    "-mrelax",
    "-fno-jump-tables",
    "-x c",
    "-Os",
    "-Wstrict-prototypes",
    "-std=gnu99",
    "-DUSE_LUFA_CONFIG_HEADER",
    "-DBOARD=BOARD_USBKEY",
    "-DF_USB=8000000UL",
    "-MMD",
    "-MP",
]

filegroup(
    name = "LufaSetup",
    srcs = glob([
        "LUFA-Setup/*.c",
        "LUFA-Setup/*.h",
    ]),
)

filegroup(
    name = "Setup",
    srcs = [
        "src/Setup/DebugSetup.h",
        "src/Setup/Delay.c",
        "src/Setup/HardwareSetup.h",
        "src/Setup/Atomic.c",
    ] + select({
        ":Motherboard": [
            "src/Setup/MotherBoardHardwareSetup.c",
            ":LufaSetup",
        ],
        ":ElasticNode": ["src/Setup/ElasticNodeHardwareSetup.c", "LUFA-Setup/Helpers.h"],
    }),
)

STD_DEPS = ["//:CommunicationModule", "@EmbeddedUtilities//:Debug"] + select({
    ":Motherboard": [
        "@LUFA//:LUFA_USB",
    ],
    "//conditions:default": [],
})

config_setting(
    name = "Motherboard",
    values = {
        "cpu": "avr",
        "define": "MCU=atmega32u4",
    },
)

config_setting(
    name = "ElasticNode",
    values = {
        "cpu": "avr",
        "define": "MCU=atmega64",
    },
)

default_embedded_binary(
    name = "SendOneFrameUsingPeripheralInterfaceOnly",
    srcs = [
        "src/SendOneFrameUsingPeripheralInterfaceOnly.c",
        ":Setup",
    ],
    copts = LUFA_COPTS,
    linkopts = LUFA_COPTS,
    deps = STD_DEPS,
)

default_embedded_binary(
    name = "WriteStringToVirtualSerial",
    srcs = [
        "src/WriteStringToVirtualSerial.c",
        ":Setup",
    ],
    copts = LUFA_COPTS,
    linkopts = LUFA_COPTS,
    deps = STD_DEPS,
)

default_embedded_binary(
    name = "ReadMrfTxStabilizationRegister",
    srcs = [
        "src/ReadMrfTxStabilizationRegister.c",
        ":Setup",
    ],
    copts = LUFA_COPTS,
    linkopts = LUFA_COPTS,
    deps = STD_DEPS,
)

default_embedded_binary(
    name = "ReadTxFifoMemory",
    srcs = [
        "src/ReadTxFifoMemory.c",
        ":Setup",
    ],
    copts = LUFA_COPTS,
    linkopts = LUFA_COPTS,
    deps = STD_DEPS,
)

default_embedded_binary(
    name = "SPITestWithoutCommunicationModuleAndLufa",
    srcs = [
        "src/SPITestWithoutCommunicationModuleAndLufa.c",
    ],
)

default_embedded_binary(
    name = "SPITestWithLufaOnly",
    srcs = [
        "src/SPITestWithLufaOnly.c",
        ":Setup",
    ],
    copts = LUFA_COPTS,
    linkopts = LUFA_COPTS,
    deps = STD_DEPS,
)

#default_embedded_binary(
#    name = "SendOneFrameBlocking",
#    srcs = [
#        "src/SendOneFrameBlocking.c",
#        ":Setup",
#    ],
#    copts = LUFA_COPTS,
#    linkopts = LUFA_COPTS,
#    deps = STD_DEPS,
#)
#
#default_embedded_binary(
#    name = "EchoMessageBlocking",
#    srcs = [
#        "src/EchoMessageBlocking.c",
#        ":Setup",
#    ],
#    copts = LUFA_COPTS,
#    linkopts = LUFA_COPTS,
#    deps = STD_DEPS,
#)

default_embedded_binaries(
    copts = LUFA_COPTS,
    linkopts = LUFA_COPTS,
    main_files = [
        "src/EchoMessageBlocking.c",
        "src/SendOneFrameBlocking.c",
    ],
    other_srcs = [":Setup"],
    deps = STD_DEPS,
)

default_embedded_binary(
    name = "PrintTXBuffer",
    srcs = [
        "src/PrintTXBuffer.c",
        ":Setup",
    ],
    copts = LUFA_COPTS,
    linkopts = LUFA_COPTS,
    deps = STD_DEPS,
)

default_embedded_binary(
    name = "PrintReceivedDataToVirtualSerial",
    srcs = [
        "src/PrintReceivedDataToVirtualSerial.c",
        ":Setup",
    ],
        copts = LUFA_COPTS,
        linkopts = LUFA_COPTS,

    deps = STD_DEPS,
)

default_embedded_binary(
    name = "WriteReadSourceAddress",
    srcs = [
        "src/WriteReadSourceAddress.c",
        ":Setup",
    ],
    copts = LUFA_COPTS,
    linkopts = LUFA_COPTS,
    deps = STD_DEPS,
)

default_embedded_binary(
    name = "BlinkLED",
    srcs = ["src/blink_led.c"],
)
