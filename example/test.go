package main

import (
	"fmt"
	"os"
	"time"

	"github.com/yushulx/goBarcodeQrSDK"
)

func main() {
	filename := "test.png"
	license := "DLS2eyJoYW5kc2hha2VDb2RlIjoiMjAwMDAxLTE2NDk4Mjk3OTI2MzUiLCJvcmdhbml6YXRpb25JRCI6IjIwMDAwMSIsInNlc3Npb25QYXNzd29yZCI6IndTcGR6Vm05WDJrcEQ5YUoifQ=="
	if len(os.Args) > 2 {
		_, err := os.Stat(os.Args[1])
		if err == nil {
			filename = os.Args[1]
		} else {
			fmt.Println("Input File not found! Use test.png instead.")
		}

		license = os.Args[2]
	} else {
		fmt.Println("reader [image] [license]")
		return
	}
	ret, _ := goBarcodeQrSDK.InitLicense(license)
	if ret != 0 {
		fmt.Printf(`initLicense("") = %d`, ret)
	}
	obj := goBarcodeQrSDK.CreateBarcodeReader()
	obj.SetParameters("{\"ImageParameter\":{\"BarcodeFormatIds\":[\"BF_ONED\",\"BF_PDF417\",\"BF_QR_CODE\",\"BF_DATAMATRIX\"],\"BarcodeFormatIds_2\":null,\"Name\":\"sts\",\"RegionDefinitionNameArray\":[\"region0\"]},\"RegionDefinition\":{\"Bottom\":100,\"Left\":0,\"MeasuredByPercentage\":1,\"Name\":\"region0\",\"Right\":100,\"Top\":0}}")
	startTime := time.Now()
	code, barcodes := obj.DecodeFile(filename)
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
		fmt.Println("--------------")
	}
}
