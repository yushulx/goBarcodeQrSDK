package main

import (
	"fmt"
	"os"
	"time"

	"github.com/yushulx/goBarcodeQrSDK/v2"
)

func main() {
	version := goBarcodeQrSDK.GetVersion()
	fmt.Println(version)

	filename := "test.png"
	license := "DLS2eyJoYW5kc2hha2VDb2RlIjoiMjAwMDAxLTE2NDk4Mjk3OTI2MzUiLCJvcmdhbml6YXRpb25JRCI6IjIwMDAwMSIsInNlc3Npb25QYXNzd29yZCI6IndTcGR6Vm05WDJrcEQ5YUoifQ=="
	template := "template.json"
	if len(os.Args) > 1 {
		for i := 1; i < len(os.Args); i++ {
			switch i {
			case 1:
				_, err := os.Stat(os.Args[1])
				if err == nil {
					filename = os.Args[1]
				} else {
					fmt.Println("Input File not found! Use test.png instead.")
				}
			case 2:
				license = os.Args[2]
			case 3:
				_, err := os.Stat(os.Args[3])
				if err == nil {
					template = os.Args[3]
				} else {
					fmt.Println("Template File not found! Use template.json instead.")
				}
			}

		}
	} else {
		fmt.Println("reader [image] [license] [template]")
		return
	}
	ret, errMsg := goBarcodeQrSDK.InitLicense(license)
	if ret != 0 {
		fmt.Println(`initLicense(): `, ret)
		fmt.Println(errMsg)
		return
	}
	obj := goBarcodeQrSDK.CreateBarcodeReader()
	ret, errMsg = obj.LoadTemplateFile(template)
	if ret != 0 {
		fmt.Println(`LoadTemplateFile(): `, ret)
		fmt.Println(errMsg)
	}
	startTime := time.Now()
	ret, barcodes := obj.DecodeFile(filename)
	elapsed := time.Since(startTime)
	fmt.Println("DecodeFile() time cost: ", elapsed)

	if ret != 0 {
		fmt.Printf(`DecodeFile() = %d`, ret)
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
