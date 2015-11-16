package main

import (
  "github.com/dynamsoftsamples/go-barcode-reader/dbr"
  "os"
)

func main() {
  if len(os.Args) == 1 { // read the default image
    dbr.Decode_file("AllSupportedBarcodeTypes.tif")
  } else {
    dbr.Decode_file(os.Args[1])
  }
}
