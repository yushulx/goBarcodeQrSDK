# Golang Barcode QR Code Reader
The project is a Golang wrapper for [Dynamsoft Barcode Reader C++ SDK](https://www.dynamsoft.com/barcode-reader/sdk-desktop-server/). You can use this module to read barcodes, QR Code (including Micro QR Code), Data Matrix, PDF417 (including Micro PDF417), Aztec Code, MaxiCode (mode 2-5), and DotCode from images and PDF files.

## Pre-requisites
* [Go v1.19](https://go.dev/dl/)
* [Dynamsoft Barcode Reader v9.4](https://www.dynamsoft.com/barcode-reader/downloads)

   You can get a free trial license from [here](https://www.dynamsoft.com/customer/license/trialLicense?product=dbr).

## Test the Module

```bash
go test
```

## Parameter Configuration
[https://www.dynamsoft.com/barcode-reader/docs/core/parameters/structure-and-interfaces-of-parameters.html?ver=latest](https://www.dynamsoft.com/barcode-reader/docs/core/parameters/structure-and-interfaces-of-parameters.html?ver=latest)

## Example

```go
package main

import (
	"fmt"
	"time"

	"github.com/yushulx/goBarcodeQrSDK"
)

func main() {
	ret, _ := goBarcodeQrSDK.InitLicense("DLS2eyJoYW5kc2hha2VDb2RlIjoiMjAwMDAxLTE2NDk4Mjk3OTI2MzUiLCJvcmdhbml6YXRpb25JRCI6IjIwMDAwMSIsInNlc3Npb25QYXNzd29yZCI6IndTcGR6Vm05WDJrcEQ5YUoifQ==")
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

Set the license key in `InitLicense()` and update the `image-file` to the path of the image file you want to decode. Then run the example.

```bash
go run .
```

