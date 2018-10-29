load("@AVR_Toolchain//:hex.bzl", "avr_minimal_copts", "generate_hex")

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

cc_binary(
    name = "SendOneFrameUsingPeripheralInterfaceOnly",
    srcs = [
        "src/SendOneFrameUsingPeripheralInterfaceOnly.c",
        "src/config.c",
        "src/config.h",
        ":LufaSetup",
    ],
    copts = avr_minimal_copts() + LUFA_COPTS,
    linkopts = avr_minimal_copts() + LUFA_COPTS,
    deps = [
        "//:CommunicationModuleLibForTest",
        "@CException//:CException",
        "@LUFA//:LUFA_USB",
    ],
)

generate_hex(
    name = "SendOneFrameUsingPeripheralInterfaceOnlyHex",
    input = ":SendOneFrameUsingPeripheralInterfaceOnly",
    mcu = "atmega328p",
)

cc_binary(
    name = "WriteStringToVirtualSerial",
    srcs = [
        "src/WriteStringToVirtualSerial.c",
        ":LufaSetup",
    ],
    copts = avr_minimal_copts() + LUFA_COPTS,
    linkopts = avr_minimal_copts() + LUFA_COPTS,
    deps = [
        "@LUFA//:LUFA_USB",
    ],
)

generate_hex(
    name = "WriteStringToVirtualSerialHex",
    input = ":WriteStringToVirtualSerial",
    mcu = "atmega32u4",
)

cc_binary(
    name = "ReadMrfTxStabilizationRegister",
    srcs = [
        "src/ReadMrfTxStabilizationRegister.c",
        ":LufaSetup",
    ],
    copts = avr_minimal_copts() + LUFA_COPTS + [
        "-Os",
        "-include stdint.h",
        "-DCEXCEPTION_T=uint8_t",
        "-DCEXCEPTION_NONE=0x00",
    ],
    linkopts = avr_minimal_copts() + LUFA_COPTS + ["-Os"],
    nocopts = "-O2",
    deps = [
        "//:CommunicationModuleLibForTest",
        "@CException//:CException",
        "@LUFA//:LUFA_USB",
    ],
)

cc_binary(
    name = "ReadTxFifoMemory",
    srcs = [
        "src/ReadTxFifoMemory.c",
        "src/config.c",
        "src/config.h",
        ":LufaSetup",
    ],
    copts = avr_minimal_copts() + LUFA_COPTS + [
        "-Os",
        "-include stdint.h",
        "-DCEXCEPTION_T=uint8_t",
        "-DCEXCEPTION_NONE=0x00",
    ],
    linkopts = avr_minimal_copts() + LUFA_COPTS + ["-Os"],
    deps = [
        "//:CommunicationModuleLibForTest",
        "@CException//:CException",
        "@LUFA//:LUFA_USB",
    ],
)

cc_binary(
    name = "SPITestWithoutCommunicationModuleAndLufa",
    srcs = [
        "src/SPITestWithoutCommunicationModuleAndLufa.c",
    ],
    copts = avr_minimal_copts(),
    linkopts = avr_minimal_copts(),
)

cc_binary(
    name = "SPITestWithLufaOnly",
    srcs = [
        "src/SPITestWithLufaOnly.c",
        ":LufaSetup",
    ],
    copts = avr_minimal_copts() + LUFA_COPTS + ["-Os"],
    linkopts = avr_minimal_copts() + LUFA_COPTS + ["-Os"],
    deps = [
        "@LUFA//:LUFA_USB",
    ],
)

cc_binary(
    name = "SendOneFrameBlocking",
    srcs = [
        "src/SendOneFrameBlocking.c",
        "src/config.c",
        "src/config.h",
        ":LufaSetup",
    ],
    copts = avr_minimal_copts() + LUFA_COPTS + [
        "-Os",
        "-include stdint.h",
        "-DCEXCEPTION_T=uint8_t",
        "-DCEXCEPTION_NONE=0x00",
    ],
    linkopts = avr_minimal_copts() + LUFA_COPTS + ["-Os"],
    nocopts = "-O2",
    deps = [
        "//:CommunicationModuleLibForTest",
        "@CException//:CException",
        "@LUFA//:LUFA_USB",
    ],
)

cc_binary(
    name = "EchoMessageBlocking",
    srcs = [
        "src/EchoMessageBlocking.c",
        "src/config.c",
        "src/config.h",
    ],
    copts = avr_minimal_copts() + LUFA_COPTS + [
        "-Os",
        "-s",
        "-fno-asynchronous-unwind-tables",
        "-ffast-math",
        "-fmerge-all-constants",
        "-fmerge-all-constants",
        "-include stdint.h",
        "-fdata-sections",
        "-ffunction-sections",
        "-DCEXCEPTION_T=uint8_t",
        "-DCEXCEPTION_NONE=0x00",
        "-fshort-enums",
    ],
    linkopts = avr_minimal_copts() + LUFA_COPTS + [
        "-Os",
        "-Xlinker --gc-sections",
    ],
    deps = [
        "//:CommunicationModule",
        "@CException//:CException",
        #        "@LUFA//:LUFA_USB",
    ],
)

cc_binary(
    name = "WriteReadSourceAddress",
    srcs = [
        "src/WriteReadSourceAddress.c",
        "src/config.c",
        "src/config.h",
        ":LufaSetup",
    ],
    copts = avr_minimal_copts() + LUFA_COPTS + [
        "-Os",
        "-include stdint.h",
        "-DCEXCEPTION_T=uint8_t",
        "-DCEXCEPTION_NONE=0x00",
    ],
    linkopts = avr_minimal_copts() + LUFA_COPTS + ["-Os"],
    nocopts = "-O2",
    deps = [
        "//:CommunicationModuleLibForTest",
        "@CException//:CException",
        "@LUFA//:LUFA_USB",
    ],
)

generate_hex(
    name = "SendOneFrameBlockingHex",
    input = ":SendOneFrameBlocking",
    mcu = "atmega32u4",
)

generate_hex(
    name = "EchoMessageBlockingHex",
    input = ":EchoMessageBlocking",
    mcu = "atmega32u4",
)

generate_hex(
    name = "WriteReadSourceAddressHex",
    input = ":WriteReadSourceAddress",
    mcu = "atmega32u4",
)

generate_hex(
    name = "ReadMrfTxStabilizationRegisterHex",
    input = ":ReadMrfTxStabilizationRegister",
    mcu = "atmega32u4",
)

generate_hex(
    name = "ReadTxFifoMemoryHex",
    input = ":ReadTxFifoMemory",
    mcu = "atmega32u4",
)

generate_hex(
    name = "SPITestWithoutCommunicationModuleAndLufaHex",
    input = ":SPITestWithoutCommunicationModuleAndLufa",
    mcu = "atmega32u4",
)

generate_hex(
    name = "SPITestWithLufaOnlyHex",
    input = ":SPITestWithLufaOnly",
    mcu = "atmega32u4",
)

cc_binary(
    name = "BlinkLED",
    srcs = ["src/blink_led.c"],
    copts = avr_minimal_copts(),
    linkopts = avr_minimal_copts(),
)

generate_hex(
    name = "BlinkLEDHex",
    input = "BlinkLED",
    mcu = "atmega32u4",
)
