package(default_visibility = ["//visibility:public"])

filegroup(
    name = "CommunicationModuleArchive",
    srcs = ["lib/libCommunicationModule.a"],
)

filegroup(
    name = "CommunicationModuleHdrs",
    srcs = glob(["CommunicationModule/*.h"]),
)

cc_library(
    name = "CommunicationModule",
    srcs = [":CommunicationModuleArchive"],
    hdrs = [":CommunicationModuleHdrs"],
    deps = [
        "@EmbeddedUtilities//:BitManipulation",
        "@EmbeddedUtilities//:Mutex",
        "@PeripheralInterface",
    ],
)
