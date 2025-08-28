#!/bin/bash

# Save the original PATH
originalPath=$LD_LIBRARY_PATH

# Get the GOPATH
GOPATH=$(go env GOPATH)

# Find the path to the shared libraries
PACKAGE_PATH=$(find "$GOPATH/pkg/mod/github.com/yushulx" -mindepth 1 -maxdepth 1 -type d | sort -r | head -n 1)
echo "PACKAGE_PATH set to $PACKAGE_PATH"
# RPATH="$PACKAGE_PATH/lib/mac"

RPATH="../../dcv/lib/mac"

echo "LIBRARY_PATH set to $LIBRARY_PATH"

TARGET="testapp"

go build -o $TARGET

if ! otool -l $TARGET | grep -q $RPATH; then
    echo "Adding rpath $RPATH to $TARGET"
    install_name_tool -add_rpath $RPATH $TARGET
else
    echo "RPATH $RPATH already exists in $TARGET"
fi

./$TARGET test.png

rm ./$TARGET