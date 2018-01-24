# Description: This file has all the functions to construct unity tests
"""
This file contains so called macros which are just functions written
in skylark (googles own constraint subset of python). For a quick
summary of what is different between python and skylark see:
https://docs.bazel.build/versions/master/skylark/language.html
"""

# i guess this could be done more elegant using the File class but it does what we want
def base_name(file_name):
    return file_name[0:-2]

def runner_base_name(file_name):
    return base_name(file_name) + "_Runner"

def runner_file_name(file_name):
    return runner_base_name(file_name) + ".c"

"""
Use the helper scripts shipped with unity to
generate a test runner for the specified file.
The genrule used here executes cmd to produce
the files specified in the outs attribute.
In the tools attribute we need to specify all the
files, that are needed during the execution of cmd.

srcs lists all input files and outs all output files.

Side fact:
The cmd is executed in bash.
The $(location ...) and $(SRC),$(OUTS) stuff is expanded
before handing it over to bash.

More Info:
https://docs.bazel.build/versions/master/be/general.html#genrule
"""
def generate_test_runner(file_name, visibility=None):
    native.genrule(
        name = runner_base_name(file_name),
        srcs = [file_name],
        outs = [runner_file_name(file_name)],
        cmd = "ruby $(location @unity//:generate_test_runner) $(SRCS) $(OUTS)",
        tools = ["@unity//:generate_test_runner",
                 "@unity//:unity-helpers",
        ],
        visibility = visibility,
    )

"""
This macro creates a cc_test rule and a genrule (that creates
the test runner) for a given file.
It adds unity as dependency so the user doesn't have to do it himself.
Additional dependencies can be specified using the deps parameter.

The source files for the test are only the *_Test.c that the user writes
and the corresponding generated *_Test_Runner.c file.
"""
def unity_test(file_name, deps=None, copts=None, size="small", visibility=None):
    generate_test_runner(file_name, visibility)
    if deps == None:
        deps = []
    if copts == None:
        copts = []
    deps += ["@unity//:unity"]
    copts += ["-Iexternal/unity/src"]
    native.cc_test(
        name = base_name(file_name),
        srcs = [file_name, runner_file_name(file_name)],
        copts = ["-Iexternal/unity/src"],
        visibility = visibility,
        deps = deps,
        size = size,
    )

"""
Convenience macro that generates a unity test for every file in a given list
using the same parameters.
"""
def generate_a_unity_test_for_every_file(file_list, deps=None, copts=None, size="small", visibility=None):
    for file in file_list:
        unity_test(
            file_name = file,
            deps = deps,
            visibility = visibility,
            copts = copts,
            size = size,
        )
