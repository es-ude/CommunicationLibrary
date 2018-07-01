load("@AVR_Toolchain//:hex.bzl", "avr_minimal_copts", "generate_hex")

cc_library(
    name = "LUFA-Setup",
    srcs = ["Descriptors.c"],
    hdrs = [
        "Descriptors.h",
        "LUFAConfig.h",
    ],
    copts = avr_minimal_copts(),
    linkopts = avr_minimal_copts(),
    deps = ["@LUFA//:LUFA_USB"],
)
