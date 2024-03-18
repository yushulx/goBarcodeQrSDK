#!/bin/bash

LIB_DIR="lib/mac"

(
    export DYLD_LIBRARY_PATH="$LIB_DIR:$DYLD_LIBRARY_PATH"
    go test
)
