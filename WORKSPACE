"""
The location of this file (even when empty) specifies the project root
for more info see https://docs.bazel.build/versions/master/build-ref.html
"""

"""
set the global repository name, this function can only be called from this file
https://docs.bazel.build/versions/master/be/functions.html#workspace
"""
workspace (
        name = "CommunicationModule",
        )

"""
Fetch unity and use the file BUILD.unity (residing in this folder) for the build.
We use the prefix new because unity isn't a bazel project, so we need to provide a BUILD file.
More info under https://docs.bazel.build/versions/master/be/workspace.html#new_http_archive
"""
new_http_archive(
  name = "Unity",
  urls = ["https://github.com/ThrowTheSwitch/Unity/archive/master.tar.gz"],
  build_file= "BUILD.Unity",
  strip_prefix = "Unity-master",
  )

new_http_archive(
  name = "CException",
  urls = ["https://github.com/ThrowTheSwitch/CException/archive/master.tar.gz"],
  build_file = "BUILD.CException",
  strip_prefix = "CException-master",
  )

http_archive(
    name = "UnityPlugin",
    urls = ["https://github.com/glencoe/BazelUnityPlugin/archive/master.tar.gz"],
    strip_prefix = "BazelUnityPlugin-master",
)

new_http_archive(
  name = "CMock",
  urls = ["https://github.com/ThrowTheSwitch/CMock/archive/master.tar.gz"],
  build_file = "BUILD.CMock",
  strip_prefix = "CMock-master",
  )

http_archive(
    name = "AVR_Toolchain",
    urls = ["https://github.com/glencoe/Bazel-AVR-Toolchain-Linux/archive/master.tar.gz"],
    strip_prefix = "Bazel-AVR-Toolchain-Linux-master"
)


"""
From the Bazel documentation at https://docs.bazel.build/versions/master/build-ref.html#packages_targets :
 The primary unit of code organization in a workspace is the package.
A package is collection of related files and a specification of the dependencies among them.

A package is defined as a directory containing a file named BUILD,
residing beneath the top-level directory in the workspace.
A package includes all files in its directory, plus all subdirectories beneath it,
except those which themselves contain a BUILD file.
"""

"""
This project has the following structure:

/
|- WORKSPACE
|- BUILD.unity
|- .bazelr
|- lib/
    |- BUILD
|- test/
    |- BUILD
    |- unity-helpers.bzl
    |- /Mocks/
        |- BUILD
|- toolchain/
    |- CROSSTOOL
    |- BUILD
"""
