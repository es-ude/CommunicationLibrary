#toolchain/BUILD
"""
I guess usually setting the visibility of a whole
package to public isn't a good idea, but the toolchain
isn't of any use except when visible to others.
"""
package(default_visibility = ["//visibility:public"])

"""
Similar to test_suite, this bundles a group of toolchains.
"""
cc_toolchain_suite(
    name = "toolchain",
    toolchains = {
        # this line maps the pair <cpu>|<compiler> to a <cc_toolchain rule>
        "avr|gcc": ":cc-compiler-avr-gcc"
    }
)

filegroup(name = "empty")

cc_toolchain(
    name = "cc-compiler-avr-gcc", # is referred in the toolchain_suite above
    all_files = ":empty",
    compiler_files = ":empty",
    cpu = "avr", # chooses the compiler to use (compiler is defined in the CROSSTOOLS file)
    dwp_files = ":empty",
    dynamic_runtime_libs = [":empty"],
    linker_files = ":empty",
    objcopy_files = ":empty",
    static_runtime_libs = [":empty"],
    strip_files = ":empty",
    supports_param_files = 1,
)
