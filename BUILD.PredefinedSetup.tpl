package(default_visibility= ["//visibility:public"])



cc_library(
    name = "CommunicationModuleSetup",
    srcs = ["libHardwareSetup.a"],
    hdrs = ["Setup/HardwareSetup.h",
            "Setup/DebugSetup.h"]
)