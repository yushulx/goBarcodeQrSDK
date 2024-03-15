package goBarcodeQrSDK

import (
	/*
	   #cgo CXXFLAGS: -std=c++11
	   #cgo CFLAGS: -I${SRCDIR}/lib -I${SRCDIR}/lib/bridge
	   #cgo linux LDFLAGS: -L${SRCDIR}/lib/linux -lbridge  -Wl,-rpath=\$$ORIGIN
	   #cgo windows LDFLAGS: -L${SRCDIR}/lib/windows -lbridge
	   #include <stdlib.h>
	   #include "bridge.h"
	*/
	"C"
)
import (
	"unsafe"
)

type Barcode struct {
	Text   string
	Format string
	X1     int
	Y1     int
	X2     int
	Y2     int
	X3     int
	Y3     int
	X4     int
	Y4     int
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

func (reader *BarcodeReader) SetParameters(params string) (int, string) {
	errorBuffer := make([]byte, 256*1024)
	ret := C.DBR_InitRuntimeSettingsWithString(reader.handler, C.CString(params), C.CM_OVERWRITE, (*C.char)(unsafe.Pointer(&errorBuffer[0])), C.int(len(errorBuffer)))
	return int(ret), string(errorBuffer)
}

func (reader *BarcodeReader) LoadTemplateFile(params string) (int, string) {
	errorBuffer := make([]byte, 256)
	ret := C.DBR_InitRuntimeSettingsWithFile(reader.handler, C.CString(params), C.CM_OVERWRITE, (*C.char)(unsafe.Pointer(&errorBuffer[0])), C.int(len(errorBuffer)))
	return int(ret), string(errorBuffer)
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
	ret = C.DBR_GetAllTextResults(reader.handler, &resultArray)

	if ret == 0 {
		if resultArray.resultsCount > 0 {
			for i := 0; i < int(resultArray.resultsCount); i++ {
				barcode := Barcode{}
				result := C.getTextResultPointer(resultArray, C.int(i))

				format := C.getFormatString(result)
				barcode.Format = C.GoString(format)

				text := C.getText(result)
				barcode.Text = C.GoString(text)

				localization := C.getLocalizationPointer(result)
				barcode.X1 = int(localization.x1)
				barcode.Y1 = int(localization.y1)
				barcode.X2 = int(localization.x2)
				barcode.Y2 = int(localization.y2)
				barcode.X3 = int(localization.x3)
				barcode.Y3 = int(localization.y3)
				barcode.X4 = int(localization.x4)
				barcode.Y4 = int(localization.y4)

				barcodes = append(barcodes, barcode)
			}
		}

		C.DBR_FreeTextResults(&resultArray)
	}

	return int(ret), barcodes
}

// GetVersion func
func GetVersion() string {
	version := C.DBR_GetVersion()
	return C.GoString(version)
}
