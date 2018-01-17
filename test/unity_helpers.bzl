def base_name(file_name):
    return file_name[0:-2]

def runner_base_name(file_name):
    return base_name(file_name) + "_Runner"

def runner_file_name(file_name):
    return runner_base_name(file_name) + ".c"

def generate_test_runner(file_name, visibility=None):
    native.genrule(
        name = runner_base_name(file_name),
        srcs = [file_name],
        outs = [runner_file_name(file_name)],
        cmd = "ruby $(location @unity//:generate_test_runner) $(SRCS) $(OUTS)",
        tools = ["@unity//:generate_test_runner",
                 "@unity//:unity-helpers",
        ],
    )

def unity_test(file_name, visibility=None):
    generate_test_runner(file_name, visibility)
    native.cc_test(
        name = base_name(file_name),
        srcs = [file_name, runner_file_name(file_name)],
        copts = ["-Iexternal/unity/src"],
        visibility = visibility,
        deps = [
                "@unity//:unity",
                "//lib:CommunicationModule",
                ],
        size = "small"
    )

def generate_a_unity_test_for_every_file(file_list, visibility=None):
    for file in file_list:
        unity_test(file)
