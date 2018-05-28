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

filegroup(
    name = "TestRunnerGenerator",
    srcs = ["TestRunnerGenerator.py"],
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
        ],
    ),
    deps = [
        "//lib:CommunicationLibrary",
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
    deps = ["//lib:CommunicationModule"],
)

unity_test(
    file_name = "MRFSetAddress_Test.c",
    deps = [
        ":MockPeripheralInterface",
        "//lib:CommunicationModule",
        "@CException//:CException",
        "@CMock//:CMock",
    ],
)

"""
We have every *_Test.c seperately build as an executable.
This rule gathers a set of cc_test rules and let'peripheral the user
execute each of them in a single bazel command.
If no cc_test rules are specified it just gathers all
cc_test rules in it'peripheral package.
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
