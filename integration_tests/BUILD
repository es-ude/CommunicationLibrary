load("@AvrToolchain//:helpers.bzl",
 "default_embedded_binaries",
  "default_embedded_binary",
  "mcu_avr_gcc_flag",
  "cpu_frequency_flag")

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
        "@AvrToolchain//config:Motherboard": [
            "src/Setup/MotherBoardHardwareSetup.c",
            ":LufaSetup",
        ],
        "@AvrToolchain//config:ElasticNode": [
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
    "@AvrToolchain//config:Motherboard": [
        "@LUFA//:LUFA_USB",
    ],
    "//conditions:default": [],
})


default_embedded_binary(
    name = "SPITestWithoutCommunicationModuleAndLufa",
        copts = mcu_avr_gcc_flag() + cpu_frequency_flag(),
        linkopts = mcu_avr_gcc_flag(),
    srcs = [
        "src/SPITestWithoutCommunicationModuleAndLufa.c",
    ],
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
        "src/NetworkMonitor.c",
    ],
    other_srcs = [":Setup"],
    deps = STD_DEPS,
)

cc_library(
    name = "MrfHardwareSetup",
    copts =LUFA_COPTS + mcu_avr_gcc_flag() + cpu_frequency_flag(),
    srcs = [":Setup"],
    hdrs = [
        "src/Setup/DebugSetup.h",
        "src/Setup/HardwareSetup.h",
    ],
    visibility = ["//visibility:public"],
    deps = STD_DEPS,
)