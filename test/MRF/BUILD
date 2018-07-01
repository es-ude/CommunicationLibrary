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
    "generate_mocks_for_every_header",
)

generate_mocks_for_every_header(
    file_list = [
        "//lib:src/Mac802154/MRF/MRFHelperFunctions.h",
        "//lib:include/Peripheral.h",
        "//lib:src/Mac802154/MRF/MRFState.h",
        "//lib:src/Mac802154/MRF/MrfIo.h",
        "//lib:src/Mac802154/FrameHeader802154.h",
        "Mac802154MRF_TestHelper.h",
    ],
    deps = ["//lib:CommunicationModuleLibForTest"],
)

unity_test(
    file_name = "Mac802154MRF_Test.c",
    mocks = [
        "//lib:src/Mac802154/MRF/MRFHelperFunctions.h",
        "//lib:src/Mac802154/MRF/MRFState.h",
        "//lib:src/Mac802154/MRF/MrfIo.h",
        "//lib:include/Peripheral.h",
        "Mac802154MRF_TestHelper.h",
    ],
    deps = [
        "//lib:CommunicationModuleLibForTest",
        "//test/Mocks:MockLibrary",
        "@CException//:CException",
        "@CMock//:CMock",
    ],
)

unity_test(
    file_name = "MrfIo_Test.c",
    mocks = [
        "MRF/Mac802154MRF_TestHelper.h",
        "//lib:include/Peripheral.h",
    ],
    deps = [
        "//lib:CommunicationModuleLibForTest",
        "@CException//:CException",
        "@CMock//:CMock",
    ],
)

unity_test(
    file_name = "MRFState_Test.c",
    mocks = [
        "//lib:src/Mac802154/FrameHeader802154.h",
        "//lib:include/Peripheral.h",
    ],
    deps = [
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
