load("@AvrToolchain//:helpers.bzl", "default_embedded_binaries", "default_embedded_binary")
load("//:upload.bzl", "upload")

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
        ":ElasticNode": [
            "src/Setup/ElasticNodeHardwareSetup.c",
            "LUFA-Setup/Helpers.h",
        ],
        "//conditions:default": [],
    }),
)

STD_DEPS = [
    "//:CommunicationModule",
    "@EmbeddedUtilities//:Debug",
] + select({
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
    name = "SPITestWithoutCommunicationModuleAndLufa",
    srcs = [
        "src/SPITestWithoutCommunicationModuleAndLufa.c",
    ],
)

default_embedded_binaries(
    copts = LUFA_COPTS,
    linkopts = LUFA_COPTS,
    main_files = [
        "src/ReadMrfTxStabilizationRegister.c",
        "src/WriteStringToVirtualSerial.c",
        "src/SendOneFrameUsingPeripheralInterfaceOnly.c",
        "src/EchoMessageBlocking.c",
        "src/SendOneFrameBlocking.c",
        "src/PrintTXBuffer.c",
        "src/ReadTxFifoMemory.c",
        "src/PrintReceivedDataToVirtualSerial.c",
        "src/WriteReadSourceAddress.c",
        "src/SPITestWithLufaOnly.c",
        "src/NetworkMonitor.c",
    ],
    other_srcs = [":Setup"],
    deps = STD_DEPS,
)

default_embedded_binary(
    name = "BlinkLED",
    srcs = ["src/blink_led.c"],
)

upload(
    name = "uploadTest",
    srcs = ["SendOneFrameUsingPeripheralInterfaceOnly"],
)
