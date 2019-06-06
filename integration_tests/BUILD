load(
    "@AvrToolchain//platforms/cpu_frequency:cpu_frequency.bzl",
    "cpu_frequency_flag",
)
load(
    "@AvrToolchain//:helpers.bzl",
    "default_embedded_binaries",
    "default_embedded_binary",
)

default_embedded_binary(
    name = "SPITestWithoutCommunicationModuleAndLufa",
    srcs = [
        "SPITestWithoutCommunicationModuleAndLufa.c",
    ],
    copts = cpu_frequency_flag(),
)

default_embedded_binary(
    name = "LufaTest",
    srcs = [
        "LufaTest.c",
    ],
    copts = cpu_frequency_flag(),
    deps = [
        "//Setup:MotherboardSetup",
        "@EmbeddedUtilities//:Debug",
    ],
)

default_embedded_binaries(
    copts = cpu_frequency_flag() +
            select({
                "//configs:debug_enabled": ["-DDEBUG=1"],
                "//conditions:default": ["-DDEBUG=0"],
            }),
    main_files = [
        "ReadMrfTxStabilizationRegister.c",
        "WriteStringToVirtualSerial.c",
        "SendOneFrameUsingPeripheralInterfaceOnly.c",
        "EchoMessageBlocking.c",
        "SendOneFrameBlocking.c",
        "PrintTXBuffer.c",
        "ReadTxFifoMemory.c",
        "PrintReceivedDataToVirtualSerial.c",
        "WriteReadSourceAddress.c",
        "SPITestWithLufaOnly.c",
        "NetworkMonitor.c",
        "ReceiveFrameWithSPIOnly.c",
        "CheckSetSourceAddresses.c",
    ],
    other_srcs = [
        "HelpersForUsageWithoutCommModule.c",
        "HelpersForUsageWithoutCommModule.h",
    ],
    deps = select({
        "@AvrToolchain//platforms/board_id:motherboard_config": ["//Setup:MotherboardSetup"],
        "@AvrToolchain//platforms/board_id:elastic_node_v3_config": ["//Setup:ElasticNodeSetup"],
    }),
)
