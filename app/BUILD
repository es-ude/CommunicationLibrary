cc_binary(
  name = "PetersBin",
  copts = ["-mmcu=$(MCU)"],
  linkopts = ["-mmcu=$(MCU)"],
  srcs = ["src/main.c"],
  deps = ["//lib:CommunicationLibrary"])

load("@AVR_Toolchain//:hex.bzl", "generate_hex")

generate_hex(
  name = "PetersBinHex",
  input = "PetersBin",
  mcu = "atmega328p"
)
