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
    uploader = "@AvrToolchain//:avrdude_upload_script",
)

default_embedded_binary(
    name = "LufaTest",
    srcs = [
        "LufaTest.c",
    ],
    copts = cpu_frequency_flag(),
    uploader = "@AvrToolchain//:avrdude_upload_script",
    deps = [
        "//Setup:MotherboardSetup",
        "@EmbeddedUtilities//:Debug",
    ],
)

default_embedded_binaries(
    copts = cpu_frequency_flag() +
            select({
                "//configs:debug_disabled": ["-DDEBUG=0"],
                "//conditions:default": ["-DDEBUG=1"],
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
    uploader = "@AvrToolchain//:avrdude_upload_script",
    deps = select({
        "@AvrToolchain//platforms/board_id:motherboard_config": ["//Setup:MotherboardSetup"],
        "@AvrToolchain//platforms/board_id:elastic_node_v3_config": ["//Setup:ElasticNodeSetup"],
        "@AvrToolchain//platforms/board_id:elastic_node_v4_config": [
            "//Setup:ElasticNodeV4Setup",
        ],
    }),
)
