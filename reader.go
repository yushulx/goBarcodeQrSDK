package goBarcodeQrSDK

import (
	"errors"
	"strings"
	"unsafe"

	/*
	   #cgo CFLAGS: -I${SRCDIR}/dcv/include
	   #cgo darwin LDFLAGS: -L${SRCDIR}/dcv/lib/mac -lbridge -Wl,-rpath,${SRCDIR}/dcv/lib/mac
	   #cgo linux LDFLAGS: -L${SRCDIR}/dcv/lib/linux -lbridge -Wl,-rpath,${SRCDIR}/dcv/lib/linux
	   #cgo windows LDFLAGS: -L${SRCDIR}/dcv/lib/win -lbridge
	   #include <stdlib.h>
	   #include "bridge.h"
	*/
	"C"
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
	PageId int // Page ID for multi-page documents (PDF, TIFF)
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
	errorBuffer := make([]byte, 256)
	c_params := C.CString(params)
	defer C.free(unsafe.Pointer(c_params))

	ret := C.DBR_InitRuntimeSettingsWithString(reader.handler, c_params, C.int(2), (*C.char)(unsafe.Pointer(&errorBuffer[0])), C.int(len(errorBuffer)))
	return int(ret), string(errorBuffer)
}

func (reader *BarcodeReader) LoadTemplateFile(params string) (int, string) {
	errorBuffer := make([]byte, 256)
	c_params := C.CString(params)
	defer C.free(unsafe.Pointer(c_params))

	ret := C.DBR_InitRuntimeSettingsWithFile(reader.handler, c_params, C.int(2), (*C.char)(unsafe.Pointer(&errorBuffer[0])), C.int(len(errorBuffer)))
	return int(ret), string(errorBuffer)
}

func (reader *BarcodeReader) processResults(resultArray *C.BarcodeResultArrayC) []Barcode {
	var barcodes = []Barcode{}

	if resultArray != nil && resultArray.count > 0 {
		// Convert C array to Go slice
		results := (*[1 << 28]C.BarcodeResultC)(unsafe.Pointer(resultArray.results))[:resultArray.count:resultArray.count]

		for i := 0; i < int(resultArray.count); i++ {
			barcode := Barcode{}
			result := &results[i]

			barcode.Format = C.GoString(result.format)
			barcode.Text = C.GoString(result.text)

			barcode.X1 = int(result.x1)
			barcode.Y1 = int(result.y1)
			barcode.X2 = int(result.x2)
			barcode.Y2 = int(result.y2)
			barcode.X3 = int(result.x3)
			barcode.Y3 = int(result.y3)
			barcode.X4 = int(result.x4)
			barcode.Y4 = int(result.y4)
			barcode.PageId = int(result.pageId)

			barcodes = append(barcodes, barcode)
		}
	}

	if resultArray != nil {
		C.DBR_FreeTextResults(&resultArray)
	}
	return barcodes
}

func (reader *BarcodeReader) DecodeFile(filePath string) ([]Barcode, error) {
	c_filePath := C.CString(filePath)
	defer C.free(unsafe.Pointer(c_filePath))

	var resultArray *C.BarcodeResultArrayC
	errorBuffer := make([]byte, 256)
	ret := C.DBR_DecodeFile(reader.handler, c_filePath, &resultArray, (*C.char)(unsafe.Pointer(&errorBuffer[0])), C.int(len(errorBuffer)))

	// Process results even if there's a warning message
	barcodes := reader.processResults(resultArray)

	// Check for error message
	errorMsg := strings.TrimRight(string(errorBuffer), "\x00")
	if ret != 0 && len(barcodes) == 0 {
		// Only return error if no barcodes were found
		if errorMsg == "" {
			errorMsg = "Decoding failed"
		}
		return barcodes, errors.New(errorMsg)
	}

	// Return barcodes with warning if applicable
	if errorMsg != "" {
		// Could log warning here: fmt.Println("Warning:", errorMsg)
	}

	return barcodes, nil
}

func (reader *BarcodeReader) DecodeStream(data []byte) ([]Barcode, error) {
	if len(data) == 0 {
		return []Barcode{}, errors.New("empty data buffer")
	}

	cData := (*C.uchar)(unsafe.Pointer(&data[0]))
	length := C.int(len(data))

	var resultArray *C.BarcodeResultArrayC
	errorBuffer := make([]byte, 256)
	ret := C.DBR_DecodeFileInMemory(reader.handler, cData, length, &resultArray, (*C.char)(unsafe.Pointer(&errorBuffer[0])), C.int(len(errorBuffer)))

	// Process results even if there's a warning message
	barcodes := reader.processResults(resultArray)

	// Check for error message
	errorMsg := strings.TrimRight(string(errorBuffer), "\x00")
	if ret != 0 && len(barcodes) == 0 {
		// Only return error if no barcodes were found
		if errorMsg == "" {
			errorMsg = "Decoding failed"
		}
		return barcodes, errors.New(errorMsg)
	}

	// Return barcodes with warning if applicable
	if errorMsg != "" {
		// Could log warning here: fmt.Println("Warning:", errorMsg)
	}

	return barcodes, nil
}

// GetVersion func
func GetVersion() string {
	version := C.DBR_GetVersion()
	return C.GoString(version)
}
