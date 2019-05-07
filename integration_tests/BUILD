load(
    "@AvrToolchain//:helpers.bzl",
    "cpu_frequency_flag",
    "default_embedded_binaries",
    "default_embedded_binary",
    "mcu_avr_gcc_flag",
)

LUFA_COPTS = [
    "-Iexternal/LUFA/Demos/Device/ClassDriver/VirtualSerial/Config",
    "-isystem external/LUFA/",
    "-Iexternal/LUFA/Demos/Device/ClassDriver/VirtualSerial/Config/",
]

filegroup(
    name = "LufaSetup",
    srcs = [
        "LUFA-Setup/Helpers.c",
        "LUFA-Setup/Helpers.h",
    ],
)

filegroup(
    name = "SetupHdrs",
    srcs = [
        "src/Setup/DebugSetup.h",
        "src/Setup/HardwareSetup.h",
    ],
    visibility = ["//visibility:public"],
)

config_setting(
    name = "debug_enabled",
    define_values = {
        "debug_enabled": "true",
    },
)

STD_DEPS = [
    "@EmbeddedUtilities//:Debug",
] + select({
    "@AvrToolchain//config:Motherboard": [
        ":MotherboardSetup",
        "@LUFA//:LUFA_USB",
    ],
    "@AvrToolchain//config:ElasticNode": [
        ":ElasticNodeSetup",
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
    copts = mcu_avr_gcc_flag() + cpu_frequency_flag(),
    linkopts = mcu_avr_gcc_flag(),
    deps = [
        ":MotherboardSetup",
        "@EmbeddedUtilities//:Debug",
        "@LUFA//:LUFA_USB",
    ],
)

default_embedded_binaries(
    copts = mcu_avr_gcc_flag() + cpu_frequency_flag() +
            select({
                ":debug_enabled": ["-DDEBUG=1"],
                "//conditions:default": ["-DDEBUG=0"],
            }),
    linkopts = mcu_avr_gcc_flag(),
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
    deps = select({
        "@AvrToolchain//config:Motherboard": [":MotherboardSetup"],
        "@AvrToolchain//config:ElasticNode": [":ElasticNodeSetup"],
    }),
)

cc_library(
    name = "MotherboardSetup",
    srcs = [
        "src/Setup/Delay.c",
        "src/Setup/MotherBoardHardwareSetup.c",
    ],
    hdrs = [
        "src/Setup/DebugSetup.h",
        "src/Setup/HardwareSetup.h",
    ],
    copts = mcu_avr_gcc_flag() + cpu_frequency_flag(),
    visibility = ["//visibility:public"],
    deps = [
        ":Atomic",
        ":DebugMotherBoard",
        "//:CommunicationModule",
    ],
)

cc_library(
    name = "DebugMotherBoard",
    srcs = [
        "src/Setup/DebugOutputMotherBoard.c",
        ":LufaSetup",
    ],
    hdrs = [
        "src/Setup/DebugSetup.h",
    ],
    copts = mcu_avr_gcc_flag() + cpu_frequency_flag() + LUFA_COPTS,
    deps = [
        "@EmbeddedUtilities//:Debug",
        "@LUFA//:VirtualSerial",
    ],
)

cc_library(
    name = "ElasticNodeSetup",
    srcs = [
        "src/Setup/Delay.c",
        "src/Setup/ElasticNodeHardwareSetup.c",
    ],
    hdrs = [
        "src/Setup/DebugSetup.h",
        "src/Setup/HardwareSetup.h",
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
        "src/Setup/Atomic.c",
    ],
    copts = mcu_avr_gcc_flag() + cpu_frequency_flag(),
    visibility = ["//visibility:public"],
    deps = [
        "@EmbeddedUtilities//:Mutex",
    ],
    alwayslink = True,
)
