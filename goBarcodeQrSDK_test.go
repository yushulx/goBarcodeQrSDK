package goBarcodeQrSDK

import (
	"fmt"
	"os"
	"testing"
	"time"
)

func TestInitLicense(t *testing.T) {
	ret, _ := InitLicense("DLS2eyJoYW5kc2hha2VDb2RlIjoiMjAwMDAxLTE2NDk4Mjk3OTI2MzUiLCJvcmdhbml6YXRpb25JRCI6IjIwMDAwMSIsInNlc3Npb25QYXNzd29yZCI6IndTcGR6Vm05WDJrcEQ5YUoifQ==")
	if ret != 0 {
		t.Fatalf(`initLicense("") = %d`, ret)
	}
}

func TestCreateBarcodeReader(t *testing.T) {
	obj := CreateBarcodeReader()
	if obj == nil {
		t.Fatalf(`Failed to create instance`)
	}
}

func TestDestroyBarcodeReader(t *testing.T) {
	obj := CreateBarcodeReader()
	DestroyBarcodeReader(obj)
}

func TestSetParameters(t *testing.T) {
	obj := CreateBarcodeReader()

	// Load template.json file content
	templateData, err := os.ReadFile("template.json")
	if err != nil {
		t.Fatalf("Failed to read template.json: %v", err)
	}

	ret, _ := obj.SetParameters(string(templateData))
	if ret != 0 {
		t.Fatalf(`SetParameters() = %d`, ret)
	}
}

func TestLoadTemplateFile(t *testing.T) {
	obj := CreateBarcodeReader()
	ret, _ := obj.LoadTemplateFile("template.json")
	if ret != 0 {
		t.Fatalf(`LoadTemplateFile() = %d`, ret)
	}
}

func TestDecodeFile(t *testing.T) {
	obj := CreateBarcodeReader()

	// Load template.json file content
	templateData, err := os.ReadFile("template.json")
	if err != nil {
		t.Fatalf("Failed to read template.json: %v", err)
	}

	obj.SetParameters(string(templateData))
	ret, _ := obj.DecodeFile("test.png")
	if ret != 0 {
		t.Fatalf(`DecodeFile() = %d`, ret)
	}
}

func TestApp(t *testing.T) {
	ret, _ := InitLicense("DLS2eyJoYW5kc2hha2VDb2RlIjoiMjAwMDAxLTE2NDk4Mjk3OTI2MzUiLCJvcmdhbml6YXRpb25JRCI6IjIwMDAwMSIsInNlc3Npb25QYXNzd29yZCI6IndTcGR6Vm05WDJrcEQ5YUoifQ==")
	if ret != 0 {
		t.Fatalf(`initLicense("") = %d`, ret)
	}
	obj := CreateBarcodeReader()

	// Load template.json file content
	templateData, err := os.ReadFile("template.json")
	if err != nil {
		t.Fatalf("Failed to read template.json: %v", err)
	}

	obj.SetParameters(string(templateData))
	startTime := time.Now()
	code, barcodes := obj.DecodeFile("test.png")
	elapsed := time.Since(startTime)
	fmt.Println("DecodeFile() time cost: ", elapsed)

	if code != 0 {
		t.Fatalf(`DecodeFile() = %d`, code)
	}

	for _, barcode := range barcodes {
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

func TestGetVersion(t *testing.T) {
	ret := GetVersion()
	fmt.Println(ret)
}

func TestDecodeStream(t *testing.T) {
	obj := CreateBarcodeReader()

	// Load template.json file content
	templateData, err := os.ReadFile("template.json")
	if err != nil {
		t.Fatalf("Failed to read template.json: %v", err)
	}

	obj.SetParameters(string(templateData))
	data, err := os.ReadFile("test.png")
	if err != nil {
		t.Fatalf("Unable to read file: %v", err)
	}

	ret, barcodes := obj.DecodeStream(data)
	if ret != 0 {
		t.Fatalf(`DecodeFile() = %d`, ret)
	}

	for _, barcode := range barcodes {
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
