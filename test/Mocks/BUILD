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


cc_library(
    name = "MockLibrary",
    srcs = [":MockModulesFiles"],
    hdrs = [
        ":MockModulesHeaders",
    ],
    visibility = ["//visibility:public"],
    deps = ["//lib:CommunicationModule"],
    linkstatic = 1, #not sure why we set that, but bazel recommended it
)
