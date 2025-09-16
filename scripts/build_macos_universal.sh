#!/bin/bash
# Build Universal Binary for macOS (ARM64 + x86_64)
# Creates a universal binary that works on both Apple Silicon and Intel Macs

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
            echo "  --clean    Clean build directories before building"
            echo "  --debug    Build in Debug configuration (default: Release)"
            echo "  --verbose  Enable verbose output"
            echo ""
            echo "This script builds universal binaries for both ARM64 and x86_64 architectures."
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

function write_warning() {
    echo -e "\033[1;33mWARNING: $1\033[0m"
}

echo -e "\033[1;33m🏗️  Building Universal goBarcodeQrSDK for macOS\033[0m"
echo -e "\033[1;33m============================================\033[0m"

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

if ! command -v lipo &> /dev/null; then
    write_error "lipo not found. Please install Xcode command line tools: xcode-select --install"
    exit 1
fi

# Check if we're in the right directory
if [ ! -f "go.mod" ]; then
    write_error "Please run this script from the project root directory"
    exit 1
fi

# Clean build directories if requested
if [ "$CLEAN" = true ]; then
    write_status "Cleaning build directories..."
    rm -rf build_arm64 build_x86_64 build
fi

# Create separate build directories for each architecture
write_status "Creating build directories..."
mkdir -p build_arm64 build_x86_64

# Build for ARM64 (Apple Silicon)
write_status "Building for ARM64 (Apple Silicon)..."
cd build_arm64

CMAKE_ARGS=(".." "-DCMAKE_OSX_ARCHITECTURES=arm64")
if [ "$VERBOSE" = true ]; then
    CMAKE_ARGS+=(--verbose)
fi

cmake "${CMAKE_ARGS[@]}"
if [ $? -ne 0 ]; then
    write_error "CMake configuration failed for ARM64"
    exit 1
fi

make -j$(sysctl -n hw.ncpu)
if [ $? -ne 0 ]; then
    write_error "ARM64 build failed"
    exit 1
fi

cd ..

# Verify ARM64 library was created
ARM64_LIB="dcv/lib/mac/libbridge.dylib"
if [ ! -f "$ARM64_LIB" ]; then
    write_error "ARM64 bridge library not found at $ARM64_LIB"
    exit 1
fi

# Copy ARM64 library to temporary location
cp "$ARM64_LIB" "libbridge_arm64.dylib"
write_success "ARM64 build completed"

# Build for x86_64 (Intel Macs)
write_status "Building for x86_64 (Intel Macs)..."
cd build_x86_64

CMAKE_ARGS=(".." "-DCMAKE_OSX_ARCHITECTURES=x86_64")
if [ "$VERBOSE" = true ]; then
    CMAKE_ARGS+=(--verbose)
fi

cmake "${CMAKE_ARGS[@]}"
if [ $? -ne 0 ]; then
    write_error "CMake configuration failed for x86_64"
    exit 1
fi

make -j$(sysctl -n hw.ncpu)
if [ $? -ne 0 ]; then
    write_error "x86_64 build failed"
    exit 1
fi

cd ..

# Verify x86_64 library was created
X86_64_LIB="dcv/lib/mac/libbridge.dylib"
if [ ! -f "$X86_64_LIB" ]; then
    write_error "x86_64 bridge library not found at $X86_64_LIB"
    exit 1
fi

# Copy x86_64 library to temporary location
cp "$X86_64_LIB" "libbridge_x86_64.dylib"
write_success "x86_64 build completed"

# Create universal binary using lipo
write_status "Creating universal binary..."
lipo -create "libbridge_arm64.dylib" "libbridge_x86_64.dylib" -output "$ARM64_LIB"
if [ $? -ne 0 ]; then
    write_error "Failed to create universal binary"
    exit 1
fi

# Clean up temporary files
rm -f "libbridge_arm64.dylib" "libbridge_x86_64.dylib"

# Verify the universal binary
write_status "Verifying universal binary..."
ARCH_INFO=$(lipo -info "$ARM64_LIB")
echo "  $ARCH_INFO"

if [[ "$ARCH_INFO" == *"arm64"* ]] && [[ "$ARCH_INFO" == *"x86_64"* ]]; then
    write_success "Universal binary created successfully"
else
    write_error "Universal binary verification failed"
    exit 1
fi

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

echo ""
write_success "Universal build completed successfully!"
echo -e "\033[1;36m📦 Built artifacts:\033[0m"
echo -e "  - Universal bridge library: dcv/lib/mac/libbridge.dylib"
echo -e "  - Command-line: example/command-line/barcode-reader"
echo -e "  - Web server: example/web/web-server"
echo ""
echo -e "\033[1;36m🔍 Library info:\033[0m"
file "$ARM64_LIB"
echo ""
echo -e "\033[1;36m💡 Run examples with:\033[0m"
echo -e "  ./example/command-line/barcode-reader <image_file>"
echo -e "  ./example/web/web-server"

# Clean up build directories (optional)
write_status "Cleaning up temporary build directories..."
rm -rf build_arm64 build_x86_64

echo ""
echo -e "\033[1;32m✅ Universal binary is ready for distribution!\033[0m"
echo -e "\033[1;36m   Works on both Apple Silicon (M1/M2/M3) and Intel Macs\033[0m"