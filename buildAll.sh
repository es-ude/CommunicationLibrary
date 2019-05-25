#!/bin/bash

function build_and_zip {
    bazel build :"$1"Zip
    if [ ! -z "$OUTPUT_DIR" ]; then
        mkdir -p "$OUTPUT_DIR"/"$1"/
        cp -f bazel-genfiles/pkg.tar.gz "$OUTPUT_DIR"/"$1"/"$1".tar.gz
    fi
}

# build_and_zip $1 native
build_and_zip $1
