#!/bin/bash

bazel build pkg
mkdir -p "$OUTPUT_DIR"/"$1"/
cp -f bazel-bin/pkg.tar.gz "$OUTPUT_DIR"/"$1"/"$1"-"$2".tar.gz
