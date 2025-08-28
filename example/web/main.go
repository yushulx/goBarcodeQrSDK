package main

import (
	"encoding/base64"
	"encoding/json"
	"fmt"
	"log"
	"net/http"
	"strings"
	"time"

	"github.com/yushulx/goBarcodeQrSDK/v2"
)

func uploadHandler(w http.ResponseWriter, r *http.Request) {
	if r.Method != "POST" {
		http.Error(w, "Only POST method is allowed", http.StatusMethodNotAllowed)
		return
	}

	// Define a struct to decode the JSON payload
	var data struct {
		Image string `json:"image"` // Field where the base64 image data will be stored
	}

	// Decode the JSON body
	if err := json.NewDecoder(r.Body).Decode(&data); err != nil {
		http.Error(w, "Error decoding JSON body", http.StatusBadRequest)
		return
	}

	// Decode the base64 string to []byte
	imgData, err := base64.StdEncoding.DecodeString(data.Image[strings.IndexByte(data.Image, ',')+1:])
	if err != nil {
		http.Error(w, "Error decoding base64 image", http.StatusBadRequest)
		return
	}

	obj := goBarcodeQrSDK.CreateBarcodeReader()
	template := "template.json"
	var ret, errMsg = obj.LoadTemplateFile(template)
	if ret != 0 {
		fmt.Println(`LoadTemplateFile(): `, ret)
		fmt.Println(errMsg)
	}
	startTime := time.Now()
	ret, barcodes := obj.DecodeStream(imgData)
	elapsed := time.Since(startTime)
	fmt.Println("DecodeStream() time cost: ", elapsed)

	if ret != 0 {
		fmt.Printf(`DecodeStream() = %d`, ret)
	}

	defer goBarcodeQrSDK.DestroyBarcodeReader(obj)

	w.Header().Set("Content-Type", "application/json")
	if err := json.NewEncoder(w).Encode(barcodes); err != nil {
		// Handle error
		http.Error(w, err.Error(), http.StatusInternalServerError)
		return
	}
}

func main() {
	version := goBarcodeQrSDK.GetVersion()
	fmt.Println(version)

	license := "DLS2eyJoYW5kc2hha2VDb2RlIjoiMjAwMDAxLTE2NDk4Mjk3OTI2MzUiLCJvcmdhbml6YXRpb25JRCI6IjIwMDAwMSIsInNlc3Npb25QYXNzd29yZCI6IndTcGR6Vm05WDJrcEQ5YUoifQ=="

	ret, errMsg := goBarcodeQrSDK.InitLicense(license)
	if ret != 0 {
		fmt.Println(`initLicense(): `, ret)
		fmt.Println(errMsg)
		return
	}

	// Serve static files from the "static" directory.
	fs := http.FileServer(http.Dir("./static"))
	http.Handle("/", fs)

	// Additional routes such as "/upload" can be defined here.
	http.HandleFunc("/upload", uploadHandler)

	// Listen on port 2024.
	log.Println("Listening on :2024...")
	http.ListenAndServe(":2024", nil)
}
