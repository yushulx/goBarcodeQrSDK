#!/bin/bash

# Save the original PATH
originalPath=$LD_LIBRARY_PATH

# Get the GOPATH
GOPATH=$(go env GOPATH)

# Find the path to the shared libraries
PACKAGE_PATH=$(find "$GOPATH/pkg/mod/github.com/yushulx" -mindepth 1 -maxdepth 1 -type d | sort -r | head -n 1)
echo "PACKAGE_PATH set to $PACKAGE_PATH"
LIBRARY_PATH="$PACKAGE_PATH/dcv/lib/linux"

echo "LIBRARY_PATH set to $LIBRARY_PATH"

export LD_LIBRARY_PATH="$LIBRARY_PATH:$originalPath"

# Run your Go application
go run main.go 

# Restore the original PATH
export LD_LIBRARY_PATH=$originalPath