"""
The location of this file (even when empty) specifies the project root
for more info see https://docs.bazel.build/versions/master/build-ref.html
"""

"""
set the global repository name, this function can only be called from this file
https://docs.bazel.build/versions/master/be/functions.html#workspace
"""

workspace(
    name = "CommunicationModule",
)

load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")
load("@bazel_tools//tools/build_defs/repo:git.bzl", "git_repository")

"""
Fetch unity and use the file BUILD.unity (residing in this folder) for the build.
We use the prefix new because unity isn't a bazel project, so we need to provide a BUILD file.
More info under https://docs.bazel.build/versions/master/be/workspace.html#new_http_archive
"""

git_repository(
    name = "EmbeddedSystemsBuildScripts",
    commit = "9e838def5abde8bad8829da3ae13c640ef0a7cc5",
    remote = "ssh://git@bitbucket.es.uni-due.de:7999/fks/bazel-avr-toolchain-linux.git",
    shallow_since = "1555999913 +0200",
)

http_archive(
    name = "Unity",
    build_file = "@EmbeddedSystemsBuildScripts//:BUILD.Unity",
    strip_prefix = "Unity-master",
    urls = ["https://github.com/ThrowTheSwitch/Unity/archive/master.tar.gz"],
)

http_archive(
    name = "CException",
    build_file = "@EmbeddedSystemsBuildScripts//:BUILD.CException",
    strip_prefix = "CException-master",
    urls = ["https://github.com/ThrowTheSwitch/CException/archive/master.tar.gz"],
)

http_archive(
    name = "UnityPlugin",
    strip_prefix = "BazelUnityPlugin-develop",
    urls = ["https://github.com/glencoe/BazelUnityPlugin/archive/develop.tar.gz"],
)

http_archive(
    name = "CMock",
    build_file = "@EmbeddedSystemsBuildScripts//:BUILD.CMock",
    strip_prefix = "CMock-master",
    urls = ["https://github.com/ThrowTheSwitch/CMock/archive/master.tar.gz"],
)

load("@EmbeddedSystemsBuildScripts//:avr.bzl", "create_avr_toolchain")

create_avr_toolchain(
    name = "AvrToolchain",
    avr_size = "/home/lukas/avr-toolchain/install/bin/avr-size",
)

http_archive(
    name = "LUFA",
    build_file = "@EmbeddedSystemsBuildScripts//:BUILD.LUFA",
    strip_prefix = "lufa-LUFA-170418",
    urls = ["http://fourwalledcubicle.com/files/LUFA/LUFA-170418.zip"],
)

git_repository(
    name = "EmbeddedUtilities",
    commit = "dba93023d12c4db6f9698a3a553340c4b8a5a256",
    remote = "ssh://git@bitbucket.es.uni-due.de:7999/im/embedded-utilities.git",
    shallow_since = "1555577581 +0200",
)

git_repository(
    name = "PeripheralInterface",
    commit = "721a0a9cca6223c1cf7e622d2793507f2598105e",
    remote = "ssh://git@bitbucket.es.uni-due.de:7999/im/peripheralinterface.git",
    shallow_since = "1555590628 +0200",
)
