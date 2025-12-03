package goBarcodeQrSDK

import (
	"bytes"
	"fmt"
	"image"
	"image/png"
	"os"
	"sync"
	"time"

	"github.com/klippa-app/go-pdfium"
	"github.com/klippa-app/go-pdfium/references"
	"github.com/klippa-app/go-pdfium/requests"
	"github.com/klippa-app/go-pdfium/webassembly"
)

var (
	pdfiumPool        pdfium.Pool
	pdfiumOnce        sync.Once
	pdfiumWorkerCount = 10
)

func SetPdfiumWorkerCount(count int) {
	pdfiumWorkerCount = count
}

func InitPdfium() {
	pdfiumOnce.Do(func() {
		// Init the PDFium library with the webassembly implementation
		// This avoids the need for cgo and pkg-config
		// Note: WebAssembly workers are created on-demand, so first few parallel
		// requests will experience sequential initialization (~250ms per worker).
		// Consider calling InitPdfium() early or pre-warming the pool if needed.
		pool, err := webassembly.Init(webassembly.Config{
			MinIdle:      pdfiumWorkerCount,     // Minimum idle workers to maintain
			MaxIdle:      pdfiumWorkerCount,     // Maximum idle workers
			MaxTotal:     pdfiumWorkerCount * 2, // Maximum total workers (active + idle)
			ReuseWorkers: true,                  // Reuse workers for better performance
		})
		if err != nil {
			fmt.Printf("Failed to init pdfium: %v\n", err)
			return
		}
		pdfiumPool = pool

		// Warm up the pool by pre-creating workers in parallel
		instances := make([]pdfium.Pdfium, pdfiumWorkerCount)
		var wg sync.WaitGroup
		for i := 0; i < pdfiumWorkerCount; i++ {
			wg.Add(1)
			go func(index int) {
				defer wg.Done()
				inst, err := pool.GetInstance(time.Second * 30)
				if err != nil {
					fmt.Printf("Failed to warm up pdfium pool: %v\n", err)
					return
				}
				instances[index] = inst
			}(i)
		}
		wg.Wait()

		// Return all instances to pool
		for _, inst := range instances {
			if inst != nil {
				inst.Close()
			}
		}
	})
}

func (reader *BarcodeReader) DecodePDF(filePath string) ([]Barcode, error) {
	if pdfiumPool == nil {
		InitPdfium()
	}
	if pdfiumPool == nil {
		return nil, fmt.Errorf("pdfium not initialized")
	}

	// Load the PDF file into a byte array
	pdfBytes, err := os.ReadFile(filePath)
	if err != nil {
		return nil, fmt.Errorf("failed to read PDF file: %w", err)
	}

	return reader.DecodePDFStream(pdfBytes)
}

func (reader *BarcodeReader) DecodePDFStream(data []byte) ([]Barcode, error) {
	if pdfiumPool == nil {
		InitPdfium()
	}
	if pdfiumPool == nil {
		return nil, fmt.Errorf("pdfium not initialized")
	}

	// Get an instance from the pool
	pdfiumInstance, err := pdfiumPool.GetInstance(time.Second * 30)
	if err != nil {
		return nil, fmt.Errorf("failed to get pdfium instance: %w", err)
	}
	defer pdfiumInstance.Close()

	// Load the PDF from memory
	doc, err := pdfiumInstance.OpenDocument(&requests.OpenDocument{
		File: &data,
	})
	if err != nil {
		return nil, fmt.Errorf("failed to open PDF: %w", err)
	}
	defer pdfiumInstance.FPDF_CloseDocument(&requests.FPDF_CloseDocument{
		Document: doc.Document,
	})

	return reader.decodePDFDocument(pdfiumInstance, doc.Document)
}

func (reader *BarcodeReader) decodePDFDocument(pdfiumInstance pdfium.Pdfium, docRef references.FPDF_DOCUMENT) ([]Barcode, error) {
	// Get page count
	pageCountResp, err := pdfiumInstance.FPDF_GetPageCount(&requests.FPDF_GetPageCount{
		Document: docRef,
	})
	if err != nil {
		return nil, fmt.Errorf("failed to get page count: %w", err)
	}

	var allBarcodes []Barcode

	for i := 0; i < pageCountResp.PageCount; i++ {
		// Render page
		// Scale 3.0 (approx 216 DPI) for better barcode detection
		scale := 3.0

		// Get page size
		pageSize, err := pdfiumInstance.FPDF_GetPageSizeByIndex(&requests.FPDF_GetPageSizeByIndex{
			Document: docRef,
			Index:    i,
		})
		if err != nil {
			continue
		}

		width := int(pageSize.Width * scale)
		height := int(pageSize.Height * scale)

		// Create bitmap
		renderResp, err := pdfiumInstance.FPDFBitmap_Create(&requests.FPDFBitmap_Create{
			Width:  width,
			Height: height,
			Alpha:  1, // Use alpha channel (BGRA)
		})
		if err != nil {
			continue
		}

		// Fill background with white (0xFFFFFFFF)
		pdfiumInstance.FPDFBitmap_FillRect(&requests.FPDFBitmap_FillRect{
			Bitmap: renderResp.Bitmap,
			Left:   0,
			Top:    0,
			Width:  width,
			Height: height,
			Color:  0xFFFFFFFF,
		})

		// Render page to bitmap
		_, err = pdfiumInstance.FPDF_RenderPageBitmap(&requests.FPDF_RenderPageBitmap{
			Bitmap: renderResp.Bitmap,
			Page: requests.Page{
				ByIndex: &requests.PageByIndex{
					Document: docRef,
					Index:    i,
				},
			},
			StartX: 0,
			StartY: 0,
			SizeX:  width,
			SizeY:  height,
			Rotate: 0,
			Flags:  0x10, // FPDF_ANNOT
		})
		if err != nil {
			pdfiumInstance.FPDFBitmap_Destroy(&requests.FPDFBitmap_Destroy{Bitmap: renderResp.Bitmap})
			continue
		}

		// Get bitmap buffer
		bufferResp, err := pdfiumInstance.FPDFBitmap_GetBuffer(&requests.FPDFBitmap_GetBuffer{
			Bitmap: renderResp.Bitmap,
		})
		if err != nil {
			pdfiumInstance.FPDFBitmap_Destroy(&requests.FPDFBitmap_Destroy{Bitmap: renderResp.Bitmap})
			continue
		}

		// Convert BGRA data to RGBA format for image encoding
		rgbaData := make([]byte, len(bufferResp.Buffer))
		for j := 0; j < len(bufferResp.Buffer); j += 4 {
			rgbaData[j] = bufferResp.Buffer[j+2]   // R
			rgbaData[j+1] = bufferResp.Buffer[j+1] // G
			rgbaData[j+2] = bufferResp.Buffer[j]   // B
			rgbaData[j+3] = bufferResp.Buffer[j+3] // A
		}

		// Convert RGBA data to image.RGBA
		img := image.NewRGBA(image.Rect(0, 0, width, height))
		copy(img.Pix, rgbaData)

		// Encode to PNG
		buf := new(bytes.Buffer)
		if err := png.Encode(buf, img); err != nil {
			pdfiumInstance.FPDFBitmap_Destroy(&requests.FPDFBitmap_Destroy{Bitmap: renderResp.Bitmap})
			continue
		}

		// Decode PNG using DecodeStream
		barcodes, err := reader.DecodeStream(buf.Bytes())
		if err == nil {
			for j := range barcodes {
				barcodes[j].PageId = i
			}
			allBarcodes = append(allBarcodes, barcodes...)
		}

		pdfiumInstance.FPDFBitmap_Destroy(&requests.FPDFBitmap_Destroy{Bitmap: renderResp.Bitmap})
	}

	return allBarcodes, nil
}
