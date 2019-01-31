load("@AVR_Toolchain//:helpers.bzl", "default_embedded_binary")

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

default_embedded_binary(
    name = "SendOneFrameUsingPeripheralInterfaceOnly",
    srcs = [
        "src/SendOneFrameUsingPeripheralInterfaceOnly.c",
        "src/Setup/HardwareSetup.h",
        "src/Setup/MotherBoardHardwareSetup.c",
        ":LufaSetup",
    ],
    copts = LUFA_COPTS,
    linkopts = LUFA_COPTS,
    deps = [
        "//:CommunicationModule",
        "@LUFA//:LUFA_USB",
    ],
)

default_embedded_binary(
    name = "WriteStringToVirtualSerial",
    srcs = [
        "src/WriteStringToVirtualSerial.c",
        ":LufaSetup",
    ],
    copts = LUFA_COPTS,
    linkopts = LUFA_COPTS,
    deps = [
        "@LUFA//:LUFA_USB",
    ],
)

default_embedded_binary(
    name = "ReadMrfTxStabilizationRegister",
    srcs = [
        "src/ReadMrfTxStabilizationRegister.c",
        ":LufaSetup",
    ],
    copts = LUFA_COPTS,
    linkopts = LUFA_COPTS,
    deps = [
        "//:CommunicationModule",
        "@LUFA//:LUFA_USB",
    ],
)

default_embedded_binary(
    name = "ReadTxFifoMemory",
    srcs = [
        "src/ReadTxFifoMemory.c",
        "src/Setup/HardwareSetup.h",
        "src/Setup/MotherBoardHardwareSetup.c",
        ":LufaSetup",
    ],
    copts = LUFA_COPTS,
    linkopts = LUFA_COPTS,
    deps = [
        "//:CommunicationModule",
        "@LUFA//:LUFA_USB",
    ],
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
        ":LufaSetup",
    ],
    copts = LUFA_COPTS,
    linkopts = LUFA_COPTS,
    deps = [
        "@LUFA//:LUFA_USB",
    ],
)

default_embedded_binary(
    name = "SendOneFrameBlocking",
    srcs = [
        "src/SendOneFrameBlocking.c",
        "src/Setup/HardwareSetup.h",
        "src/Setup/MotherBoardHardwareSetup.c",
        ":LufaSetup",
    ],
    copts = LUFA_COPTS,
    linkopts = LUFA_COPTS,
    deps = [
        "//:CommunicationModule",
        "@LUFA//:LUFA_USB",
    ],
)

default_embedded_binary(
    name = "EchoMessageBlocking",
    srcs = [
        "src/EchoMessageBlocking.c",
        "src/Setup/DebugSetup.h",
        "src/Setup/Delay.c",
        "src/Setup/HardwareSetup.h",
        "src/Setup/MotherBoardHardwareSetup.c",
    ],
    deps = [
        "//:CommunicationModule",
    ],
)

#cc_binary(
#    name = "ElasticNodeEchoMessageBlocking",
#    srcs = [
#        "src/EchoMessageBlocking.c",
#        "src/Setup/DebugSetup.h",
#        "src/Setup/Delay.c",
#        "src/Setup/ElasticNodeHardwareSetup.c",
#        "src/Setup/MrfHardwareSetup.h",
#    ],
#    copts = avr_minimal_copts(),
#    linkopts = avr_minimal_copts(),
#    deps = [
#        "//:CommunicationModule",
#        "//:Debug",
#        "@CException",
#    ],
#)
#
#cc_binary(
#    name = "PrintTXBuffer",
#    srcs = [
#        "src/PrintTXBuffer.c",
#        "src/Setup/DebugSetup.h",
#        "src/Setup/Delay.c",
#        "src/Setup/ElasticNodeHardwareSetup.c",
#        "src/Setup/MrfHardwareSetup.h",
#    ],
#    copts = avr_minimal_copts(),
#    linkopts = avr_minimal_copts(),
#    deps = [
#        "//:CommunicationModule",
#        "//:Debug",
#        "@CException",
#    ],
#)

#cc_binary(
#    name = "PrintReceivedDataToVirtualSerial",
#    srcs = [
#        "src/PrintReceivedDataToVirtualSerial.c",
#        "src/Setup/MotherBoardHardwareSetup.c",
#        "src/Setup/MrfHardwareSetup.h",
#        ":LufaSetup",
#    ],
#    copts = avr_minimal_copts() + LUFA_COPTS + CODE_SIZE_OPTIMIZATION_COPTS,
#    linkopts = avr_minimal_copts() + LUFA_COPTS + [
#        "-Xlinker --gc-sections",
#        "-Os",
#    ],
#    deps = [
#        "//:CommunicationModule",
#        "@CException",
#        "@LUFA//:LUFA_USB",
#    ],
#)

default_embedded_binary(
    name = "WriteReadSourceAddress",
    srcs = [
        "src/Setup/HardwareSetup.h",
        "src/Setup/MotherBoardHardwareSetup.c",
        "src/WriteReadSourceAddress.c",
        ":LufaSetup",
    ],
    copts = LUFA_COPTS,
    linkopts = LUFA_COPTS,
    deps = [
        "//:CommunicationModule",
        "@LUFA//:LUFA_USB",
    ],
)

default_embedded_binary(
    name = "BlinkLED",
    srcs = ["src/blink_led.c"],
)
