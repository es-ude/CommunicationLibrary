filegroup(
    name = "MockModulesSource",
    srcs = glob(["Mock*.c"])
)

filegroup(
    name = "MockModulesHeaders",
    srcs = glob(["Mock*.h"])
)

filegroup(
    name = "MockModulesFiles",
    srcs = glob(["Mock*.h", "Mock*.c"])
)

cc_inc_library(
    name = "MockLibrary",
    hdrs = [
         ":MockModulesHeaders",
    ],
    deps = [
        ":MockLibrary_impl",
        ],
    testonly = 1,
    visibility = ["//visibility:public"]
)

cc_library(
    name = "MockLibrary_impl",
    srcs = [":MockModulesFiles"],
    visibility = ["//visibility:private"],
    deps = ["//lib:CommunicationModule"],
    linkstatic = 1, #not sure why we set that, but bazel recommended it
)
