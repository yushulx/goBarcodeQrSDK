# Golang Barcode QR Code Reader
This project serves as a Golang wrapper for the [Dynamsoft Barcode Reader C++ SDK](https://www.dynamsoft.com/barcode-reader/sdk-desktop-server/). With this module, you can efficiently read various types of barcodes and symbols, including QR Codes (and Micro QR Codes), Data Matrix, PDF417 (and Micro PDF417), Aztec Code, MaxiCode (modes 2-5), and DotCode from images and PDF files.

## Prerequisites
* [Go](https://go.dev/dl/)
* [30-day free trial license](https://www.dynamsoft.com/customer/license/trialLicense/?product=dcv&package=cross-platform) for Dynamsoft Barcode Reader.

## Supported Platforms
- Windows
- Linux
- macOS

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

```go
package main

import (
	"fmt"
	"time"

	"github.com/yushulx/goBarcodeQrSDK"
)

func main() {
	ret, _ := goBarcodeQrSDK.InitLicense("LICENSE-KEY")
	if ret != 0 {
		fmt.Printf(`initLicense("") = %d`, ret)
	}
	obj := goBarcodeQrSDK.CreateBarcodeReader()
	templateData, err := os.ReadFile("template.json")
	if err != nil {
		t.Fatalf("Failed to read template.json: %v", err)
	}

	ret, _ := obj.SetParameters(string(templateData))
	startTime := time.Now()
	code, barcodes := obj.DecodeFile("image-file")
	elapsed := time.Since(startTime)
	fmt.Println("DecodeFile() time cost: ", elapsed)

	if code != 0 {
		fmt.Printf(`DecodeFile() = %d`, code)
	}

	for i := 0; i < len(barcodes); i++ {
		barcode := barcodes[i]
		fmt.Println(barcode.Text)
		fmt.Println(barcode.Format)
		fmt.Println(barcode.X1)
		fmt.Println(barcode.Y1)
		fmt.Println(barcode.X2)
		fmt.Println(barcode.Y2)
		fmt.Println(barcode.X3)
		fmt.Println(barcode.Y3)
		fmt.Println(barcode.X4)
		fmt.Println(barcode.Y4)
	}
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
