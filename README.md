# Golang Barcode QR Code Reader

[![Cross-Platform Build](https://github.com/yushulx/goBarcodeQrSDK/actions/workflows/build.yml/badge.svg)](https://github.com/yushulx/goBarcodeQrSDK/actions/workflows/build.yml)
[![Test PR](https://github.com/yushulx/goBarcodeQrSDK/actions/workflows/test-pr.yml/badge.svg)](https://github.com/yushulx/goBarcodeQrSDK/actions/workflows/test-pr.yml)
[![Go Report Card](https://goreportcard.com/badge/github.com/yushulx/goBarcodeQrSDK)](https://goreportcard.com/report/github.com/yushulx/goBarcodeQrSDK/v2)

This project serves as a Golang wrapper for the [Dynamsoft Barcode Reader C++ SDK](https://www.dynamsoft.com/barcode-reader/sdk-desktop-server/). With this module, you can efficiently read various types of barcodes and symbols, including QR Codes (and Micro QR Codes), Data Matrix, PDF417 (and Micro PDF417), Aztec Code, MaxiCode (modes 2-5), and DotCode from images and PDF files.

## Prerequisites
* [Go](https://go.dev/dl/)
* [30-day free trial license](https://www.dynamsoft.com/customer/license/trialLicense/?product=dcv&package=cross-platform) for Dynamsoft Barcode Reader.

## Building the Bridge Library

**Important**: Before using this Go module, you must build the C++ bridge library that connects Go to the Dynamsoft Barcode Reader SDK. This bridge is essential for the module to function.

### Prerequisites for Building
- **Windows**: Visual Studio 2022 with C++ build tools, CMake
- **Linux**: GCC/G++, CMake, build-essential
- **macOS**: Xcode Command Line Tools, CMake

### Quick Build (All Platforms)
Use the provided build scripts to automatically build the bridge library:

```bash
# Windows (PowerShell)
.\scripts\build.ps1

# Linux/macOS
./scripts/build.sh          # For Linux
./scripts/build_macos.sh    # For macOS
```

### Manual Build Process
If you prefer to build manually or need to customize the build:

1. **Create build directory:**
   ```bash
   mkdir build
   cd build
   ```

2. **Configure with CMake:**
   ```bash
   # Windows
   cmake .. -G "Visual Studio 17 2022" -A x64
   
   # Linux/macOS
   cmake .. -DCMAKE_BUILD_TYPE=Release
   ```

3. **Build the library:**
   ```bash
   # Windows
   cmake --build . --config Release
   
   # Linux/macOS
   cmake --build . --config Release
   ```

4. **Verify build artifacts:**
   - **Windows**: `dcv/lib/win/Release/bridge.dll`
   - **Linux**: `dcv/lib/linux/libbridge.so`
   - **macOS**: `dcv/lib/mac/libbridge.dylib`

## Supported Platforms
- Windows (x64)
- Linux (x64)
- macOS (x64/ARM64)

## Test the Module

```bash
# Windows
.\run_windows_test.ps1

# Linux
./run_linux_test.sh

## mac
chmod +x run_mac_test.sh
sudo ./run_mac_test.sh
```

## How to Use the Go Module 

**Before using the module, make sure you have built the bridge library as described in the "Building the Bridge Library" section above.**

1. Download the Go module:

	```bash
	go get github.com/yushulx/goBarcodeQrSDK
	```

2. Import the package in your Go file:

	```go
	import (
		"github.com/yushulx/goBarcodeQrSDK"
	)
	```
3. Specify the dynamic library path at runtime before running Go apps.

	- **Windows**

		```bash
		$originalPath = $env:PATH

		# Get the GOPATH
		$GOPATH = $(go env GOPATH)

		# Find the path to the shared libraries
		$PACKAGE_PATH = Get-ChildItem -Path "$GOPATH\pkg\mod\github.com\yushulx" -Directory | Sort-Object LastWriteTime -Descending | Select-Object -First 1 -ExpandProperty FullName
		Write-Host "PACKAGE_PATH set to $PACKAGE_PATH"
		$LIBRARY_PATH = "$PACKAGE_PATH\lib\windows"

		Write-Host "LIBRARY_PATH set to $LIBRARY_PATH"
		# Update PATH to include the assembly path
		$env:PATH = "$LIBRARY_PATH;" + $env:PATH
		# Write-Host "PATH set to $($env:PATH)"

		# Run your Go application
		go run test.go test.png

		$env:PATH = $originalPath
		```

	- **Linux**

		```bash
		#!/bin/bash

		# Save the original PATH
		originalPath=$LD_LIBRARY_PATH

		# Get the GOPATH
		GOPATH=$(go env GOPATH)

		# Find the path to the shared libraries
		PACKAGE_PATH=$(find "$GOPATH/pkg/mod/github.com/yushulx" -mindepth 1 -maxdepth 1 -type d | sort -r | head -n 1)
		echo "PACKAGE_PATH set to $PACKAGE_PATH"
		LIBRARY_PATH="$PACKAGE_PATH/lib/linux"

		echo "LIBRARY_PATH set to $LIBRARY_PATH"

		export LD_LIBRARY_PATH="$LIBRARY_PATH:$originalPath"

		# Run your Go application
		go run test.go test.png

		# Restore the original PATH
		export LD_LIBRARY_PATH=$originalPath
		```
		
	- **macOS**

		```bash
		#!/bin/bash

		# Save the original PATH
		originalPath=$LD_LIBRARY_PATH

		# Get the GOPATH
		GOPATH=$(go env GOPATH)

		# Find the path to the shared libraries
		PACKAGE_PATH=$(find "$GOPATH/pkg/mod/github.com/yushulx" -mindepth 1 -maxdepth 1 -type d | sort -r | head -n 1)
		echo "PACKAGE_PATH set to $PACKAGE_PATH"
		RPATH="$PACKAGE_PATH/lib/mac"

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
		```

## Quick Start
Set the license key within the `InitLicense()` function, and replace the `image-file` with the path of the image file you wish to decode.

**Note**: This SDK supports multi-page documents (PDF, TIFF). Each barcode result includes a `PageId` field indicating which page the barcode was found on.

```go
package main

import (
	"fmt"
	"os"
	"time"

	"github.com/yushulx/goBarcodeQrSDK"
)

func main() {
	// Initialize license
	ret, errMsg := goBarcodeQrSDK.InitLicense("LICENSE-KEY")
	if ret != 0 {
		fmt.Printf("License initialization failed: %d, %s\n", ret, errMsg)
	}

	// Create barcode reader
	obj := goBarcodeQrSDK.CreateBarcodeReader()
	defer goBarcodeQrSDK.DestroyBarcodeReader(obj)

	// Load template (optional)
	templateData, err := os.ReadFile("template.json")
	if err != nil {
		fmt.Printf("Failed to read template.json: %v\n", err)
	} else {
		ret, errMsg := obj.SetParameters(string(templateData))
		if ret != 0 {
			fmt.Printf("SetParameters failed: %d, %s\n", ret, errMsg)
		}
	}

	// Decode barcodes from file - NEW API follows Go conventions
	startTime := time.Now()
	barcodes, err := obj.DecodeFile("image-file")
	elapsed := time.Since(startTime)
	fmt.Println("DecodeFile() time cost:", elapsed)

	if err != nil {
		// Error might contain warnings, but barcodes could still be found
		fmt.Printf("DecodeFile warning/error: %v\n", err)
		if len(barcodes) == 0 {
			fmt.Println("No barcodes found")
			return
		}
		fmt.Printf("Found %d barcodes despite warning\n", len(barcodes))
	}

	// Process results
	for i, barcode := range barcodes {
		fmt.Printf("\nBarcode %d:\n", i+1)
		fmt.Printf("  Page ID: %d\n", barcode.PageId)  // Shows which page the barcode was found on
		fmt.Printf("  Text: %s\n", barcode.Text)
		fmt.Printf("  Format: %s\n", barcode.Format)
		fmt.Printf("  Coordinates: (%d,%d) (%d,%d) (%d,%d) (%d,%d)\n",
			barcode.X1, barcode.Y1, barcode.X2, barcode.Y2,
			barcode.X3, barcode.Y3, barcode.X4, barcode.Y4)
	}

	// Alternative: Decode from memory
	imageData, err := os.ReadFile("image-file")
	if err == nil {
		barcodes, err = obj.DecodeStream(imageData)
		if err != nil {
			fmt.Printf("DecodeStream error: %v\n", err)
		} else {
			fmt.Printf("DecodeStream found %d barcodes\n", len(barcodes))
		}
	}
}

``` 

## 📚 API Reference

### Core Functions
```go
// License management
ret, errMsg := goBarcodeQrSDK.InitLicense("LICENSE-KEY")

// Reader lifecycle
reader := goBarcodeQrSDK.CreateBarcodeReader()
goBarcodeQrSDK.DestroyBarcodeReader(reader)

// Get SDK version
version := goBarcodeQrSDK.GetVersion()
```

### Configuration
```go
// Load settings from JSON string
ret, errMsg := reader.SetParameters(jsonString)

// Load settings from file
ret, errMsg := reader.LoadTemplateFile("template.json")
```

### Barcode Detection
```go
// Decode from file
barcodes, err := reader.DecodeFile("image.png")

// Decode from memory buffer 
barcodes, err := reader.DecodeStream(imageData)
```

### Barcode Result Structure
```go
type Barcode struct {
    Text   string  // Decoded text content
    Format string  // Barcode format (QR_CODE, CODE_128, etc.)
    X1, Y1 int     // Top-left corner
    X2, Y2 int     // Top-right corner  
    X3, Y3 int     // Bottom-right corner
    X4, Y4 int     // Bottom-left corner
    PageId int     // Page number (for multi-page documents)
}
```

## Example
- [Command-line](https://github.com/yushulx/goBarcodeQrSDK/tree/main/example/command-line)
- [Web](https://github.com/yushulx/goBarcodeQrSDK/tree/main/example/web)


## Docker Build
- Build and run barcode QR code reader in Docker:

    ```bash
    docker build -t golang-barcode-qr-reader .
    docker run -it --rm golang-barcode-qr-reader
    ```
- Read barcode and QR code from a local image file:

    ```bash
    docker run -it --rm -v <image-folder>:/app golang-barcode-qr-reader reader /app/<image-file> <license-key> <template-file>
    ```

## Docker Usage
[https://hub.docker.com/repository/docker/yushulx/golang-barcode-qr-reader](https://hub.docker.com/repository/docker/yushulx/golang-barcode-qr-reader)

 ```bash
docker run -it --rm -v <image-folder>:/app yushulx/golang-barcode-qr-reader:latest reader /app/<image-file> <license-key> <template-file>
```
