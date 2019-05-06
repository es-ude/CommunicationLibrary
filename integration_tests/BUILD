load(
    "@AvrToolchain//:helpers.bzl",
    "cpu_frequency_flag",
    "default_embedded_binaries",
    "default_embedded_binary",
    "mcu_avr_gcc_flag",
)

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
    name = "SetupHdrs",
    srcs = [
        "src/Setup/HardwareSetup.h",
        "src/Setup/DebugSetup.h"
    ],
    visibility = ["//visibility:public"]
)

STD_DEPS = [
    "@EmbeddedUtilities//:Debug",
] + select({
    "@AvrToolchain//config:Motherboard": [
        ":MotherboardSetup",
        "@LUFA//:LUFA_USB",
    ],
    "@AvrToolchain//config:ElasticNode": [
        ":ElasticNodeSetup"
    ],
    "//conditions:default": [],
})

default_embedded_binary(
    name = "SPITestWithoutCommunicationModuleAndLufa",
    srcs = [
        "src/SPITestWithoutCommunicationModuleAndLufa.c",
    ],
    copts = mcu_avr_gcc_flag() + cpu_frequency_flag(),
    linkopts = mcu_avr_gcc_flag(),
)

default_embedded_binary(
    name = "LufaTest",
    srcs = [
        "src/LufaTest.c",
    ],
    deps = [
        "@LUFA//:LUFA_USB",
        "@EmbeddedUtilities//:Debug",
        ":MotherboardSetup",
    ],
    copts = mcu_avr_gcc_flag() + cpu_frequency_flag() + LUFA_COPTS,
    linkopts = LUFA_COPTS + mcu_avr_gcc_flag(),
)

default_embedded_binaries(
    copts = LUFA_COPTS + mcu_avr_gcc_flag() + cpu_frequency_flag(),
    linkopts = LUFA_COPTS + mcu_avr_gcc_flag(),
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
#        "src/NetworkMonitor.c",
    ],
    deps = select({
    "@AvrToolchain//config:Motherboard": [":MotherboardSetup"],
    "@AvrToolchain//config:ElasticNode": [":ElasticNodeSetup"],
    })
)

default_embedded_binary(
        copts = LUFA_COPTS + mcu_avr_gcc_flag() + cpu_frequency_flag(),
        linkopts = LUFA_COPTS + mcu_avr_gcc_flag(),
        deps = [
            ":MotherboardSetup",
            "@LUFA//:LUFA_USB",
            "@EmbeddedUtilities//:Debug",
        ],
        srcs = ["src/NetworkMonitor.c"],
        name = "NetworkMonitor",
)

cc_library(
    name = "MotherboardSetup",
    srcs = [
        "src/Setup/Atomic.c",
        "src/Setup/Delay.c",
        "src/Setup/MotherBoardHardwareSetup.c",
        ":LufaSetup",
    ],
    alwayslink = True,
    hdrs = [
        "src/Setup/DebugSetup.h",
        "src/Setup/HardwareSetup.h",
    ],
    copts = LUFA_COPTS + mcu_avr_gcc_flag() + cpu_frequency_flag(),
    visibility = ["//visibility:public"],
    deps = [
        "//:CommunicationModule",
        "@LUFA//:LUFA_USB",
    ],
)

cc_library(
    name = "ElasticNodeSetup",
    srcs = [
        "src/Setup/Atomic.c",
        "src/Setup/Delay.c",
        "src/Setup/ElasticNodeHardwareSetup.c",
    ],
    alwayslink = True,
    hdrs = [
        "src/Setup/DebugSetup.h",
        "src/Setup/HardwareSetup.h",
    ],
    copts = LUFA_COPTS + mcu_avr_gcc_flag() + cpu_frequency_flag(),
    visibility = ["//visibility:public"],
    deps = [
        "//:CommunicationModule",
        "@EmbeddedUtilities//:Debug",
    ],
)
