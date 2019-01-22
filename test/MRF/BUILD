# All tests for the CommunicationModule

"""
The load function is used to make the function generate_a_unity_test_for_every_file
from the file unity_helpers.bzl in the package test available here.
"""

#load("//:unity_helpers.bzl", "generate_a_unity_test_for_every_file")
load(
    "@UnityPlugin//Unity_rules:unity.bzl",
    "generate_a_unity_test_for_every_file",
    "generate_mocks_for_every_header",
    "mock",
    "unity_test",
)

generate_mocks_for_every_header(
    enforce_strict_ordering = True,
    file_list = [
        "//:src/Mac802154/MRF/MRFHelperFunctions.h",
        "//:CommunicationModule/PeripheralInterface.h",
        "//test/MRF:Mac802154MRF_TestHelper.h",
        "//:src/Mac802154/MRF/MRFState.h",
        "//:src/Mac802154/MRF/MrfIo.h",
        "//:src/Mac802154/FrameHeader802154.h",
    ],
    deps = ["//:CommunicationModuleLibForTest"],
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
    mocks = [
        "//:src/Mac802154/MRF/MRFHelperFunctions.h",
        "//:src/Mac802154/MRF/MRFState.h",
        "//:src/Mac802154/MRF/MrfIo.h",
        "//:src/Mac802154/FrameHeader802154.h",
        "//:CommunicationModule/PeripheralInterface.h",
        "//test/MRF:Mac802154MRF_TestHelper.h",
    ],
    deps = [
        "//:CommunicationModuleLibForTest",
        "@CException",
        "@CMock",
    ],
)

unity_test(
    copts = [
        "-std=gnu99",
    ],
    file_name = "MrfIo_Test.c",
    mocks = [
        "//test/MRF:Mac802154MRF_TestHelper.h",
        "//:CommunicationModule/PeripheralInterface.h",
    ],
    deps = [
        "//:CommunicationModuleLibForTest",
        "@CException",
        "@CMock",
    ],
)

unity_test(
    copts = [
        "-std=gnu99",
    ],
    file_name = "MRFState_Test.c",
    mocks = [
        "//:src/Mac802154/FrameHeader802154.h",
        "//:CommunicationModule/PeripheralInterface.h",
    ],
    deps = [
        "//:CommunicationModuleLibForTest",
        "@CException",
        "@CMock",
    ],
)
