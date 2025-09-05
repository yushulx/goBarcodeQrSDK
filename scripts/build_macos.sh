#!/bin/bash
# Build Script for macOS
# Builds the becho -e "\033[1;33mBuilding goBarcodeQrSDK for macOS\033[0m"
echo -e "\033[1;33m====================================\033[0m"dge library and Go modules

set -e

CONFIGURATION="Release"
CLEAN=false
VERBOSE=false

# Parse command line arguments
while [[ $# -gt 0 ]]; do
    case $1 in
        --clean)
            CLEAN=true
            shift
            ;;
        --debug)
            CONFIGURATION="Debug"
            shift
            ;;
        --verbose)
            VERBOSE=true
            shift
            ;;
        -h|--help)
            echo "Usage: $0 [--clean] [--debug] [--verbose]"
            echo "  --clean    Clean build directory before building"
            echo "  --debug    Build in Debug configuration (default: Release)"
            echo "  --verbose  Enable verbose output"
            exit 0
            ;;
        *)
            echo "Unknown option $1"
            exit 1
            ;;
    esac
done

function write_status() {
    echo -e "\033[1;36m>> $1\033[0m"
}

function write_success() {
    echo -e "\033[1;32mSUCCESS: $1\033[0m"
}

function write_error() {
    echo -e "\033[1;31mERROR: $1\033[0m"
}

echo -e "\033[1;33m🚀 Building goBarcodeQrSDK for macOS\033[0m"
echo -e "\033[1;33m===================================\033[0m"

# Check prerequisites
write_status "Checking prerequisites..."

if ! command -v cmake &> /dev/null; then
    write_error "CMake not found. Please install cmake: brew install cmake"
    exit 1
fi

if ! command -v go &> /dev/null; then
    write_error "Go not found. Please install Go: brew install go"
    exit 1
fi

if ! command -v gcc &> /dev/null; then
    write_error "GCC not found. Please install Xcode command line tools: xcode-select --install"
    exit 1
fi

# Check if we're in the right directory
if [ ! -f "go.mod" ]; then
    write_error "Please run this script from the project root directory"
    exit 1
fi

# Clean build directory if requested
if [ "$CLEAN" = true ] && [ -d "build" ]; then
    write_status "Cleaning build directory..."
    rm -rf build
fi

# Create build directory
if [ ! -d "build" ]; then
    write_status "Creating build directory..."
    mkdir build
fi

# Configure CMake
write_status "Configuring CMake..."
cd build

CMAKE_ARGS=("..")
if [ "$VERBOSE" = true ]; then
    CMAKE_ARGS+=(--verbose)
fi

cmake "${CMAKE_ARGS[@]}"
if [ $? -ne 0 ]; then
    write_error "CMake configuration failed"
    exit 1
fi

# Build the bridge library
write_status "Building bridge library ($CONFIGURATION)..."
BUILD_ARGS=("." "--config" "$CONFIGURATION")
if [ "$VERBOSE" = true ]; then
    BUILD_ARGS+=(--verbose)
fi

make -j$(sysctl -n hw.ncpu)
if [ $? -ne 0 ]; then
    write_error "Bridge library build failed"
    exit 1
fi

cd ..

# Verify bridge library was created
BRIDGE_LIB="dcv/lib/mac/libbridge.dylib"
if [ ! -f "$BRIDGE_LIB" ]; then
    write_error "Bridge library not found at $BRIDGE_LIB"
    exit 1
fi

write_success "Bridge library built successfully"

# Set CGO environment
export CGO_ENABLED=1

# Build and test Go module
write_status "Testing Go module..."
go mod tidy
if [ $? -ne 0 ]; then
    write_error "go mod tidy failed"
    exit 1
fi

go test -v
if [ $? -ne 0 ]; then
    write_error "Go tests failed"
    exit 1
fi

write_success "Go module tests passed"

# Build examples
write_status "Building examples..."

# Command-line example
cd "example/command-line"
go mod tidy
if [ $? -ne 0 ]; then
    write_error "Command-line example go mod tidy failed"
    exit 1
fi

go build -o "barcode-reader" .
if [ $? -ne 0 ]; then
    write_error "Command-line example build failed"
    exit 1
fi
write_success "Command-line example built"
cd ../..

# Web example
cd "example/web"
go mod tidy
if [ $? -ne 0 ]; then
    write_error "Web example go mod tidy failed"
    exit 1
fi

go build -o "web-server" .
if [ $? -ne 0 ]; then
    write_error "Web example build failed"
    exit 1
fi
write_success "Web example built"
cd ../..

# Desktop example (simple version)
if [ -f "example/desktop/main_simple.go" ]; then
    cd "example/desktop"
    
    # Use simple go.mod without OpenCV dependencies
    if [ -f "go_simple.mod" ]; then
        cp "go_simple.mod" "go.mod"
    fi
    
    go mod tidy
    if [ $? -eq 0 ]; then
        go build -o "barcode-scanner-simple" "main_simple.go"
        if [ $? -eq 0 ]; then
            write_success "Desktop simple example built"
        else
            echo -e "\033[1;33m⚠️  Desktop simple example build failed (non-critical)\033[0m"
        fi
    else
        echo -e "\033[1;33m⚠️  Desktop simple example go mod tidy failed (non-critical)\033[0m"
    fi
    cd ../..
fi

echo ""
write_success "Build completed successfully!"
echo -e "\033[1;36m📦 Built artifacts:\033[0m"
echo -e "  - Bridge library: dcv/lib/mac/libbridge.dylib"
echo -e "  - Command-line: example/command-line/barcode-reader"
echo -e "  - Web server: example/web/web-server"
if [ -f "example/desktop/barcode-scanner-simple" ]; then
    echo -e "  - Desktop app: example/desktop/barcode-scanner-simple"
fi
echo ""
echo -e "\033[1;36m💡 Run examples with:\033[0m"
echo -e "  ./example/command-line/barcode-reader <image_file>"
echo -e "  ./example/web/web-server"
echo -e "  ./example/desktop/barcode-scanner-simple <image_file>"
