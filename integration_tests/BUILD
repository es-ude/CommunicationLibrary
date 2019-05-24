load(
    "@AvrToolchain//:helpers.bzl",
    "cpu_frequency_flag",
    "default_embedded_binaries",
    "default_embedded_binary",
    "mcu_avr_gcc_flag",
)

load("@bazel_tools//tools/build_defs/pkg:pkg.bzl", "pkg_tar")

default_embedded_binary(
    name = "SPITestWithoutCommunicationModuleAndLufa",
    srcs = [
        "SPITestWithoutCommunicationModuleAndLufa.c",
    ],
    copts = mcu_avr_gcc_flag() + cpu_frequency_flag(),
    linkopts = mcu_avr_gcc_flag(),
)

default_embedded_binary(
    name = "LufaTest",
    srcs = [
        "LufaTest.c",
    ],
    copts = mcu_avr_gcc_flag() + cpu_frequency_flag(),
    linkopts = mcu_avr_gcc_flag(),
    deps = [
        "//Setup:MotherboardSetup",
        "@EmbeddedUtilities//:Debug",
    ],
)

default_embedded_binaries(
    copts = mcu_avr_gcc_flag() + cpu_frequency_flag() +
            select({
                "//configs:debug_enabled": ["-DDEBUG=1"],
                "//conditions:default": ["-DDEBUG=0"],
            }),
    linkopts = mcu_avr_gcc_flag(),
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
        "@AvrToolchain//config:Motherboard": ["//Setup:MotherboardSetup"],
        "@AvrToolchain//config:ElasticNode": ["//Setup:ElasticNodeSetup"],
    }),
)

