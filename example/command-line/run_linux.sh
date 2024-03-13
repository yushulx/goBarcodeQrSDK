#!/bin/bash

# ASSEMBLY_PATH=$(ls -d $(go env GOPATH)/pkg/mod/github.com/yushulx/*/lib)
ASSEMBLY_PATH=../../lib/linux
export LD_LIBRARY_PATH="$ASSEMBLY_PATH:$LD_LIBRARY_PATH"
echo "LD_LIBRARY_PATH set to $LD_LIBRARY_PATH"

go run test.go test.png
