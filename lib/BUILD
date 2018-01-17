config_setting(
    name = "avr",
    values = {
        "cpu": "avr",
    },
    )

cc_library(
        name = "CommunicationModule",
        srcs = glob(["*.c"]),
        hdrs = glob(["*.h"]),
        copts = select({
              ":avr": ["-mmcu=$(MCU)"],
              "//conditions:default": [],
        }),
        visibility = ["//test:__pkg__"],
        )
