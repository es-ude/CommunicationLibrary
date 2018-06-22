load("@AVR_Toolchain//:hex.bzl", "avr_minimal_copts", "generate_hex")

cc_binary(
    name = "WriteOneFrameBlocking",
    srcs = ["src/write_one_frame_blocking.c"],
    copts = avr_minimal_copts(),
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
