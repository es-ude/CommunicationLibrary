# All tests for the CommunicationModule

"""
The load function is used to make the function generate_a_unity_test_for_every_file
from the file unity_helpers.bzl in the package test available here.
"""

load(
    "@EmbeddedSystemsBuildScripts//:unity.bzl",
    "generate_a_mock_for_every_file",
    "generate_a_unity_test_for_every_file",
    "mock",
    "unity_test",
)

mock(
    name = "PeripheralInterfaceMock",
    srcs = ["@PeripheralInterface//:PeripheralInterface/PeripheralInterface.h"],
    enforce_strict_ordering = True,
    deps = ["@PeripheralInterface//:PeripheralInterfaceHdrsOnly"]
    )

mock(
    name = "FrameHeaderMock",
    srcs = ["//:src/Mac802154/MRF/FrameHeader802154.h"],
    deps = ["//:CommunicationModule"],
)

generate_a_mock_for_every_file(
    enforce_strict_ordering = True,
    file_list = [
        "//:src/Mac802154/MRF/MRFHelperFunctions.h",
        "//test/MRF:Mac802154MRF_TestHelper.h",
        "//:src/Mac802154/MRF/MRFState.h",
        "//:src/Mac802154/MRF/MrfIo.h",
    ],
    deps = [
        "//:CommunicationModule",
    ],
)

unity_test(
    copts = [
        "-Wno-unused-function",
        "-Werror=return-type",
        #        "-Werror=discarded-qualifiers",
        #        "-Wno-error=incompatible-pointer-types",
        "-funsigned-char",
        "-std=gnu99",
    ],
    file_name = "Mac802154MRF_Test.c",
    deps = [
        ":MockMRFHelperFunctions",
        ":MockMRFState",
        ":MockMrfIo",
        ":MockMac802154MRF_TestHelper",
        ":FrameHeaderMock",
        ":PeripheralInterfaceMock",
        "@CException",
        "@CMock",
    ],
)

unity_test(
    copts = [
        "-std=gnu99",
    ],
    file_name = "MrfIo_Test.c",
    deps = [
                ":MockMac802154MRF_TestHelper",

        ":PeripheralInterfaceMock",
        "//:CommunicationModule",
        "@CException",
        "@CMock",
    ],
)

unity_test(
    copts = [
        "-std=gnu99",
    ],
    file_name = "MRFState_Test.c",
    deps = [
        ":FrameHeaderMock",
        ":PeripheralInterfaceMock",
        "//:CommunicationModule",
        "@CException",
        "@CMock",
    ],
)
