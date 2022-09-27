package goBarcodeQrSDK

import (
	"unsafe"

	// #include <stdlib.h>
	// #include <lib/DynamsoftBarcodeReader.h>
	// #include <lib/DynamsoftCommon.h>
	// #include <lib/bridge.c>
	// #cgo LDFLAGS: -L ./lib -lDynamsoftBarcodeReader -Wl,-rpath=./lib
	"C"
)

type Barcode struct {
	text   string
	format string
	x1     int
	y1     int
	x2     int
	y2     int
	x3     int
	y3     int
	x4     int
	y4     int
}

func InitLicense(license string) (int, string) {
	c_license := C.CString(license)
	defer C.free(unsafe.Pointer(c_license))

	errorBuffer := make([]byte, 256)
	ret := C.DBR_InitLicense(c_license, (*C.char)(unsafe.Pointer(&errorBuffer[0])), C.int(len(errorBuffer)))

	return int(ret), string(errorBuffer)
}

func CreateBarcodeReader() *BarcodeReader {
	handler := C.DBR_CreateInstance()
	if handler == nil {
		return nil
	}
	return &BarcodeReader{handler: handler}
}

func DestroyBarcodeReader(obj *BarcodeReader) {
	C.DBR_DestroyInstance(obj.handler)
}

type BarcodeReader struct {
	handler unsafe.Pointer
}

func (reader *BarcodeReader) SetParameters(params string) int {
	errorBuffer := make([]byte, 256)
	ret := C.DBR_InitRuntimeSettingsWithString(reader.handler, C.CString(params), C.CM_OVERWRITE, (*C.char)(unsafe.Pointer(&errorBuffer[0])), C.int(len(errorBuffer)))
	return int(ret)
}

func (reader *BarcodeReader) DecodeFile(filePath string) (int, []Barcode) {
	c_filePath := C.CString(filePath)
	defer C.free(unsafe.Pointer(c_filePath))
	template := C.CString("")
	defer C.free(unsafe.Pointer(template))

	var barcodes = []Barcode{}
	ret := C.DBR_DecodeFile(reader.handler, c_filePath, template)

	if ret != 0 {
		return int(ret), barcodes
	}

	var resultArray *C.TextResultArray
	C.DBR_GetAllTextResults(reader.handler, &resultArray)

	if resultArray.resultsCount > 0 {
		for i := 0; i < int(resultArray.resultsCount); i++ {
			barcode := Barcode{}
			result := C.getTextResultPointer(resultArray, C.int(i))

			format := C.getFormatString(result)
			barcode.format = C.GoString(format)

			text := C.getText(result)
			barcode.text = C.GoString(text)

			localization := C.getLocalizationPointer(result)
			barcode.x1 = int(localization.x1)
			barcode.y1 = int(localization.y1)
			barcode.x2 = int(localization.x2)
			barcode.y2 = int(localization.y2)
			barcode.x3 = int(localization.x3)
			barcode.y3 = int(localization.y3)
			barcode.x4 = int(localization.x4)
			barcode.y4 = int(localization.y4)

			barcodes = append(barcodes, barcode)
		}
	}

	C.DBR_FreeTextResults(&resultArray)
	return int(ret), barcodes
}
