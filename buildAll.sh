#!/bin/bash

function build_and_zip {
    bazel build :CommunicationModuleZip --config=$1
    if [ ! -z "$OUTPUT_DIR" ]; then
        mkdir -p "$OUTPUT_DIR"
        cp -f bazel-genfiles/CommunicationModule.zip "$OUTPUT_DIR"/communicationmodule.zip
    fi
}

build_and_zip
