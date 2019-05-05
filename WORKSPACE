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

http_archive(
    name = "EmbeddedSystemsBuildScripts",
    type = "zip",
    urls = ["http://artifactory.es.uni-due.de:8081/artifactory/libs-release-local/im/EmbeddedSystemsBuildScripts.zip"],
)

load("@EmbeddedSystemsBuildScripts//:avr.bzl", "avr_toolchain")
avr_toolchain()

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
    name = "CMock",
    build_file = "@EmbeddedSystemsBuildScripts//:BUILD.CMock",
    strip_prefix = "CMock-master",
    urls = ["https://github.com/ThrowTheSwitch/CMock/archive/master.tar.gz"],
)

http_archive(
    name = "LUFA",
    build_file = "@AvrToolchain//:BUILD.LUFA",
    strip_prefix = "lufa-LUFA-170418",
    urls = ["http://fourwalledcubicle.com/files/LUFA/LUFA-170418.zip"],
)

git_repository(
    name = "EmbeddedUtilities",
    commit = "69f06616dbf1b7042ac8d5b56b25bf436a9b3ab7",
    remote = "ssh://git@bitbucket.es.uni-due.de:7999/im/embedded-utilities.git",
)

git_repository(
    name = "PeripheralInterface",
    commit = "8ccfe7b6e84673304f943be81f94ce8404e8c833",
    remote = "ssh://git@bitbucket.es.uni-due.de:7999/im/peripheralinterface.git",
)
