# All tests for the CommunicationModule

"""
The load function is used to make the function generate_a_unity_test_for_every_file
from the file unity_helpers.bzl in the package test available here.
"""

#load("//:unity_helpers.bzl", "generate_a_unity_test_for_every_file")
load(
    "@UnityPlugin//Unity_rules:unity.bzl",
    "generate_a_unity_test_for_every_file",
    "unity_test",
    "mock",
)

"""
This macro is still somewhat hacky. But generally it mimicks
the behaviour of ceedling to some extend. It uses the helper scripts
shipped with unity to generate for every file specified a test runner
for every file specified and  a cc_test (the bazel test entity for c/c++).
"""

generate_a_unity_test_for_every_file(
    file_list = glob(
        ["*_Test.c"],
        exclude = [
            "Mac802154MRF_Test.c",
            "MRFSetAddress_Test.c",
            "MRFState_Test.c",
            "MrfIo_Test.c",
        ],
    ),
    deps = [
        "//lib:CommunicationModuleLibForTest",
        "//test/Mocks:MockLibrary",
        "@CException//:CException",
        "@CMock//:CMock",
    ],
)

mock(
    name = "MockPeripheralInterface",
    file = "//lib:include/Peripheral.h",
)

mock(
    name = "MockMRFHelperFunctions",
    file = "//lib:src/Mac802154/MRF/MRFHelperFunctions.h",
    deps = ["//lib:CommunicationModuleLibForTest"],
)

mock(
    name = "MockMac802154Test_Helper",
    file = "Mac802154MRF_TestHelper.h",
    deps = ["//lib:CommunicationModuleLibForTest"],
)

mock(
    name = "MockMRFState",
    file = "//lib:src/Mac802154/MRF/MRFState.h",
    deps = ["//lib:CommunicationModuleLibForTest"],
)

unity_test(
    file_name = "Mac802154MRF_Test.c",
    deps = [
        ":MockMRFHelperFunctions",
        ":MockMRFState",
        ":MockMac802154Test_Helper",
        ":MockMrfIo",
        ":MockPeripheralInterface",
        "//lib:CommunicationModuleLibForTest",
        "//test/Mocks:MockLibrary",
        "@CException//:CException",
        "@CMock//:CMock",
    ],
)

unity_test(
    file_name = "MrfIo_Test.c",
    deps = [
        ":MockMac802154Test_Helper",
        ":MockPeripheralInterface",
        "//lib:CommunicationModule",
        "@CException//:CException",
        "@CMock//:CMock",
    ],
)

mock(
    name = "MockMrfIo",
    file = "//lib:src/Mac802154/MRF/MrfIo.h",
    deps = ["//lib:CommunicationModuleLibForTest"],
)

mock(
    name = "MockFrameHeader802154",
    file = "//lib:src/Mac802154/FrameHeader802154.h",
)

unity_test(
    file_name = "MRFState_Test.c",
    deps = [
        ":MockFrameHeader802154",
        ":MockPeripheralInterface",
        "//lib:CommunicationModuleLibForTest",
        "@CException//:CException",
        "@CMock//:CMock",
    ],
)

"""
We have every *_Test.c seperately build as an executable.
This rule gathers a set of cc_test rules and let's the user
execute each of them in a single bazel command.
If no cc_test rules are specified it just gathers all
cc_test rules in it's package.
So by running
    bazel test test:all
you can run every test available.

Side fact:
Bazel won't even execute the tests, if none of
them has changed. It just prints the cached test logs (it tells
you they are the cached ones).

Q: But what if i read some file inside a test and this dummy
   file has changed, but not the test executable?
A: Well... if you have you're test depending on a file you'd
   have to tell bazel so. Otherwise you're test could not even
   access the file. Bazel trys to make sure that everything your
   test depends on is explicitly specified, hence known to bazel.
"""

test_suite(
    name = "ALL",
)
