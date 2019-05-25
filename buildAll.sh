#!/bin/bash

#!/bin/bash

function build_and_zip {
    bazel build :"$1"Zip --config=$2
    if [ ! -z "$OUTPUT_DIR" ]; then
        mkdir -p "$OUTPUT_DIR"/"$1"/
        cp -f bazel-genfiles/"$1".zip "$OUTPUT_DIR"/"$1"/"$1"-"$2".zip
    fi
}

# build_and_zip $1 native
build_and_zip $1 avr
