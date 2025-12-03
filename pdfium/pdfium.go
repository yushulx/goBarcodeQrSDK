package pdfium

import (
	"errors"
	"image"
	"image/draw"
	"os"
	"time"

	gopdfium "github.com/klippa-app/go-pdfium"
	"github.com/klippa-app/go-pdfium/references"
	"github.com/klippa-app/go-pdfium/requests"
	"github.com/klippa-app/go-pdfium/webassembly"
)

var (
	pool     gopdfium.Pool
	instance gopdfium.Pdfium
)

// Init initializes the PDFium library using WebAssembly (no CGO, true multi-threading)
// workerCount: number of PDFium worker instances (0 = default 4)
// workerBinPath: ignored for WebAssembly (kept for API compatibility)
func Init(workerCount int, workerBinPath string) error {
	if workerCount <= 0 {
		workerCount = 4
	}

	var err error
	// Initialize WebAssembly PDFium (no CGO required, embeds PDFium binary)
	pool, err = webassembly.Init(webassembly.Config{
		MinIdle:  workerCount,
		MaxIdle:  workerCount,
		MaxTotal: workerCount,
	})
	if err != nil {
		return err
	}

	instance, err = pool.GetInstance(time.Second * 30)
	if err != nil {
		return err
	}

	return nil
}

// Destroy cleans up PDFium resources
func Destroy() {
	if instance != nil {
		instance.Close()
	}
	if pool != nil {
		pool.Close()
	}
}

// Document represents a PDF document
type Document struct {
	docRef references.FPDF_DOCUMENT
}

// Open opens a PDF document from file path
// For WebAssembly, files are automatically read into memory
func Open(filePath string) (*Document, error) {
	if instance == nil {
		return nil, errors.New("PDFium not initialized, call pdfium.Init() first")
	}

	// Read file into memory (required for WebAssembly)
	data, err := os.ReadFile(filePath)
	if err != nil {
		return nil, err
	}

	// Open document from memory
	doc, err := instance.OpenDocument(&requests.OpenDocument{
		File: &data,
	})
	if err != nil {
		return nil, err
	}

	return &Document{docRef: doc.Document}, nil
}

// OpenFromMemory opens a PDF from byte slice
func OpenFromMemory(data []byte) (*Document, error) {
	if instance == nil {
		return nil, errors.New("PDFium not initialized, call pdfium.Init() first")
	}

	// Open document from memory
	doc, err := instance.OpenDocument(&requests.OpenDocument{
		File: &data,
	})
	if err != nil {
		return nil, err
	}

	return &Document{docRef: doc.Document}, nil
}

// Close closes the document and frees resources
func (d *Document) Close() {
	if d.docRef != "" {
		instance.FPDF_CloseDocument(&requests.FPDF_CloseDocument{
			Document: d.docRef,
		})
		d.docRef = ""
	}
}

// PageCount returns the number of pages in the document
func (d *Document) PageCount() int {
	if d.docRef == "" {
		return 0
	}

	pageCount, err := instance.FPDF_GetPageCount(&requests.FPDF_GetPageCount{
		Document: d.docRef,
	})
	if err != nil {
		return 0
	}

	return pageCount.PageCount
}

// RenderPage renders a page to RGBA bitmap
// pageIndex: 0-based page index
// scale: rendering scale (1.0 = 72 DPI, 2.0 = 144 DPI, 3.0 = 216 DPI)
// Returns: RGBA byte array, width, height, error
// TRUE PARALLEL: Each worker instance can render independently!
func (d *Document) RenderPage(pageIndex int, scale float64) ([]byte, int, int, error) {
	if d.docRef == "" {
		return nil, 0, 0, errors.New("document is closed")
	}

	// Calculate DPI from scale (72 DPI is the base)
	dpi := int(72 * scale)

	// Render page using go-pdfium
	pageRender, err := instance.RenderPageInDPI(&requests.RenderPageInDPI{
		Page: requests.Page{
			ByIndex: &requests.PageByIndex{
				Document: d.docRef,
				Index:    pageIndex,
			},
		},
		DPI: dpi,
	})
	if err != nil {
		return nil, 0, 0, err
	}
	defer pageRender.Cleanup()

	// Convert image.Image to RGBA bytes
	img := pageRender.Result.Image
	bounds := img.Bounds()
	width := bounds.Dx()
	height := bounds.Dy()

	rgbaData := make([]byte, width*height*4)
	for y := 0; y < height; y++ {
		for x := 0; x < width; x++ {
			r, g, b, a := img.At(x, y).RGBA()
			idx := (y*width + x) * 4
			rgbaData[idx+0] = uint8(r >> 8)
			rgbaData[idx+1] = uint8(g >> 8)
			rgbaData[idx+2] = uint8(b >> 8)
			rgbaData[idx+3] = uint8(a >> 8)
		}
	}

	return rgbaData, width, height, nil
}

// RenderPageToImage renders a page directly to image.Image
// This is more efficient than RenderPage when you need an image
func (d *Document) RenderPageToImage(pageIndex int, scale float64) (image.Image, error) {
	if d.docRef == "" {
		return nil, errors.New("document is closed")
	}

	dpi := int(72 * scale)

	pageRender, err := instance.RenderPageInDPI(&requests.RenderPageInDPI{
		Page: requests.Page{
			ByIndex: &requests.PageByIndex{
				Document: d.docRef,
				Index:    pageIndex,
			},
		},
		DPI: dpi,
	})
	if err != nil {
		return nil, err
	}
	defer pageRender.Cleanup()

	// Create a copy of the image to ensure it persists after Cleanup
	src := pageRender.Result.Image
	bounds := src.Bounds()
	dst := image.NewRGBA(bounds)
	draw.Draw(dst, bounds, src, bounds.Min, draw.Src)

	return dst, nil
}
