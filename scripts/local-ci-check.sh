#!/bin/bash

# Local CI Check Script
# This script mimics what GitHub Actions will do, so you can test locally

set -e

echo "🔍 Local CI Check for goBarcodeQrSDK"
echo "=================================="

# Check if we're in the right directory
if [ ! -f "go.mod" ]; then
    echo "❌ Error: Please run this script from the project root directory"
    exit 1
fi

echo "📦 Checking Go module..."
go mod tidy
go mod verify

echo "🔧 Building bridge library..."
mkdir -p build
cd build

# Detect platform and build accordingly
if [[ "$OSTYPE" == "msys" || "$OSTYPE" == "win32" ]]; then
    echo "🪟 Detected Windows platform"
    cmake .. -G "Visual Studio 17 2022" -A x64
    cmake --build . --config Release
elif [[ "$OSTYPE" == "darwin"* ]]; then
    echo "🍎 Detected macOS platform"
    # Ensure CMake is available
    if ! command -v cmake &> /dev/null; then
        echo "CMake not found. Please install CMake first:"
        echo "brew install cmake"
        exit 1
    fi
    cmake ..
    make
else
    echo "🐧 Detected Linux platform"
    # Ensure CMake is available
    if ! command -v cmake &> /dev/null; then
        echo "CMake not found. Please install CMake first:"
        echo "sudo apt-get update && sudo apt-get install -y cmake build-essential"
        exit 1
    fi
    cmake ..
    make
fi

cd ..

echo "🧪 Running tests..."
CGO_ENABLED=1 go test -v

echo "🔨 Building examples..."
cd example/command-line
go mod tidy
go build .
echo "✅ Command-line example built successfully"

cd ../web
go mod tidy
go build .
echo "✅ Web example built successfully"

cd ../..

echo ""
echo "🎉 All checks passed! Your changes should work in CI."
echo "💡 To create a release, run: git tag v2.x.x && git push origin v2.x.x"
