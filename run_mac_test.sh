#!/bin/bash

RPATH="./lib/mac"
TARGET="testapp"

go test -c -o $TARGET

if ! otool -l $TARGET | grep -q $RPATH; then
    echo "Adding rpath $RPATH to $TARGET"
    install_name_tool -add_rpath $RPATH $TARGET
else
    echo "RPATH $RPATH already exists in $TARGET"
fi

./$TARGET

rm ./$TARGET