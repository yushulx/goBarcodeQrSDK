#!/bin/bash
# Universal Build Script
# Automatically detects platform and runs the appropriate build script

set -e

# Detect platform
PLATFORM=""
case "$(uname -s)" in
    Linux*)     PLATFORM="linux";;
    Darwin*)    PLATFORM="macos";;
    CYGWIN*|MINGW*|MSYS*) PLATFORM="windows";;
    *)          echo "Unsupported platform: $(uname -s)"; exit 1;;
esac

echo "Detected platform: $PLATFORM"

# Get script directory
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

case $PLATFORM in
    "linux")
        echo "Running Linux build script..."
        exec "$SCRIPT_DIR/build.sh" "$@"
        ;;
    "macos")
        echo "Running macOS build script..."
        exec "$SCRIPT_DIR/build_macos.sh" "$@"
        ;;
    "windows")
        echo "Please run scripts/build.ps1 for Windows"
        exit 1
        ;;
esac
