load("@AVR_Toolchain//:hex.bzl", "avr_minimal_copts", "generate_hex")

LUFA_COPTS = [
    "-Iexternal/LUFA/Demos/Device/ClassDriver/VirtualSerial/Config",
    #    "-I./LUFA/Drivers/USB/Core/AVR8",
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
]

cc_binary(
    name = "WriteOneFrameBlocking",
    srcs = ["src/write_one_frame_blocking.c"],
    copts = avr_minimal_copts() + LUFA_COPTS,
    linkopts = avr_minimal_copts(),
    deps = [
        "//lib:CommunicationModuleLibForTest",
        "@CException//:CException",
    ],
)

generate_hex(
    name = "WriteOneFrameBlockingHex",
    input = ":WriteOneFrameBlocking",
    mcu = "atmega328p",
)

cc_binary(
    name = "WriteStringToVirtualSerial",
    srcs = [
        "src/WriteStringToVirtualSerial.c",
    ] + glob([
        "LUFA-Setup/*.h",
        "LUFA-Setup/*.c",
    ]),
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
