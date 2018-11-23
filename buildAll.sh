#!/bin/bash

function build_and_zip {
    bazel build :CommunicationModule_$1_zip --config=$1
    if [ ! -z "$OUTPUT_DIR" ]; then
        mkdir -p "$OUTPUT_DIR"
        cp -f bazel-genfiles/CommunicationModule_$1.zip "$OUTPUT_DIR"/
    fi
}

build_and_zip atmega32u4
build_and_zip atmega328p