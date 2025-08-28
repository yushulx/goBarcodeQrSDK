#!/bin/bash

# Script to prepare examples for CI (removes replace directives)
# This ensures CI tests the actual published module

echo "Preparing examples for CI testing..."

# Function to remove replace directive from go.mod
remove_replace() {
    local file=$1
    if [ -f "$file" ]; then
        echo "  Processing $file"
        # Remove replace directive line
        sed -i '/^replace.*=> \.\.\//d' "$file"
        echo "  Removed replace directive from $file"
    fi
}

# Process example directories
if [ -d "example/command-line" ]; then
    remove_replace "example/command-line/go.mod"
fi

if [ -d "example/web" ]; then
    remove_replace "example/web/go.mod"
fi

echo "Examples prepared for CI testing"
echo "Examples will now use the published module from GitHub"
