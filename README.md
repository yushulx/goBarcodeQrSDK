# Golang Barcode QR Code Reader
This project serves as a Golang wrapper for the [Dynamsoft Barcode Reader C++ SDK](https://www.dynamsoft.com/barcode-reader/sdk-desktop-server/). With this module, you can efficiently read various types of barcodes and symbols, including QR Codes (and Micro QR Codes), Data Matrix, PDF417 (and Micro PDF417), Aztec Code, MaxiCode (modes 2-5), and DotCode from images and PDF files.

## Prerequisites
* [Go](https://go.dev/dl/)
* [Dynamsoft Barcode Reader v9.x trial license](https://www.dynamsoft.com/customer/license/trialLicense?product=dbr).

## Test the Module

```bash
# Windows
run_windows_test.ps1

# Linux
run_linux_test.sh
```

## Parameter Configuration
[https://www.dynamsoft.com/barcode-reader/docs/core/parameters/structure-and-interfaces-of-parameters.html](https://www.dynamsoft.com/barcode-reader/docs/core/parameters/structure-and-interfaces-of-parameters.html)

## Example
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
	obj.SetParameters("{\"ImageParameter\":{\"BarcodeFormatIds\":[\"BF_ONED\",\"BF_PDF417\",\"BF_QR_CODE\",\"BF_DATAMATRIX\"],\"BarcodeFormatIds_2\":null,\"Name\":\"sts\",\"RegionDefinitionNameArray\":[\"region0\"]},\"RegionDefinition\":{\"Bottom\":100,\"Left\":0,\"MeasuredByPercentage\":1,\"Name\":\"region0\",\"Right\":100,\"Top\":0}}")
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
