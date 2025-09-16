# Build Scripts

This directory contains build scripts for all supported platforms.

## Quick Start

### Windows
```powershell
.\scripts\build.ps1
```

### Linux/macOS
```bash
# Linux
./scripts/build.sh

# macOS (single architecture)
./scripts/build_macos.sh

# macOS Universal Binary (recommended)
./scripts/build_macos_universal.sh

# Cross-platform universal script
./scripts/build_universal.sh
```

## Platform-Specific Scripts

### Windows (`build.ps1`)
- **Requirements**: Visual Studio 2022, CMake, Go
- **Features**: 
  - Builds bridge library using MSVC
  - Runs tests with proper DLL path configuration
  - Builds all examples
  - No emoji output (CI-friendly)

**Usage:**
```powershell
# Basic build
.\scripts\build.ps1

# Clean build
.\scripts\build.ps1 -Clean

# Debug build with verbose output
.\scripts\build.ps1 -Configuration Debug -Verbose
```

### Linux (`build.sh`)
- **Requirements**: GCC, CMake, Go, build-essential
- **Features**:
  - Builds bridge library using GCC
  - Sets proper LD_LIBRARY_PATH for testing
  - Builds all examples

**Usage:**
```bash
# Basic build
./scripts/build.sh

# Clean build
./scripts/build.sh --clean

# Debug build with verbose output
./scripts/build.sh --debug --verbose
```

### macOS (`build_macos.sh`)
- **Requirements**: Xcode Command Line Tools, CMake, Go
- **Features**:
  - Builds bridge library using Clang
  - Sets proper DYLD_LIBRARY_PATH for testing
  - Handles macOS-specific linking
  - Builds for current system architecture only

**Usage:**
```bash
# Basic build
./scripts/build_macos.sh

# Clean build
./scripts/build_macos.sh --clean

# Debug build
./scripts/build_macos.sh --debug
```

### macOS Universal (`build_macos_universal.sh`)
- **Requirements**: Xcode Command Line Tools, CMake, Go, lipo
- **Features**:
  - Builds bridge library for both ARM64 and x86_64
  - Creates universal binary using `lipo`
  - Tests compatibility on current system
  - **Recommended for distribution**

**Usage:**
```bash
# Basic universal build
./scripts/build_macos_universal.sh

# Clean universal build
./scripts/build_macos_universal.sh --clean

# Debug universal build
./scripts/build_macos_universal.sh --debug --verbose
```

### Universal (`build_universal.sh`)
- **Purpose**: Automatically detects platform and runs appropriate script
- **Usage**: `./scripts/build_universal.sh [options]`

## CI/CD Scripts

### Local CI Check (`local-ci-check.ps1`)
Mimics GitHub Actions workflow for local testing:
- Builds bridge library
- Runs tests with proper DLL paths
- Builds examples
- Validates module integrity

## Build Outputs

After successful build, you'll find:

### Windows
- Bridge library: `dcv/lib/win/Release/bridge.dll`
- Command-line tool: `example/command-line/barcode-reader.exe`
- Web server: `example/web/web-server.exe`
- Desktop app: `example/desktop/barcode-scanner-simple.exe`

### Linux
- Bridge library: `dcv/lib/linux/libbridge.so`
- Command-line tool: `example/command-line/barcode-reader`
- Web server: `example/web/web-server`
- Desktop app: `example/desktop/barcode-scanner-simple`

### macOS
- Bridge library: `dcv/lib/mac/libbridge.dylib` (Universal binary: ARM64 + x86_64)
- Command-line tool: `example/command-line/barcode-reader`
- Web server: `example/web/web-server`
- Desktop app: `example/desktop/barcode-scanner-simple`

**Note**: When using `build_macos_universal.sh`, the resulting `libbridge.dylib` is a universal binary that works on both Apple Silicon and Intel Macs.
