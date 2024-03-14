#!/bin/bash

# Save the original PATH
originalPath=$LD_LIBRARY_PATH

# Get the GOPATH
GOPATH=$(go env GOPATH)

# Find the path to the shared libraries
PACKAGE_PATH=$(find "$GOPATH/pkg/mod/github.com/yushulx" -mindepth 1 -maxdepth 1 -type d | sort -r | head -n 1)
echo "PACKAGE_PATH set to $PACKAGE_PATH"

ASSEMBLY_PATH="$PACKAGE_PATH/lib/linux"
echo "ASSEMBLY_PATH set to $ASSEMBLY_PATH"

export LD_LIBRARY_PATH="$ASSEMBLY_PATH:$originalPath"

# Run your Go application
go run test.go test.png

# Restore the original PATH
export LD_LIBRARY_PATH=$originalPath