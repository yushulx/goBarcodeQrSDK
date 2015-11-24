# Golang Barcode Reader for Windows
The sample demonstrates how to create a Golang Barcode Reader by wrapping Dynamsoft Barcode Reader SDK with SWIG on Windows.

![golang barcode reader](http://www.codepool.biz/wp-content/uploads/2015/11/go_barcode_reader.png)

Download & Installation
------------
* [mingw-w64][1]
* [SWIG][2]
* [Go][3]
* [Dynamsoft Barcode Reader][4]

Basic Steps
-----------
1. Set GOPATH as the location of your workspace. E.g. **set GOPATH=f:\go-project**
2. Create your package path. E.g. ``mkdir %GOPATH%\src\github.com\dynamsoftsamples\go-barcode-reader\dbr``
3. Copy **DynamsoftBarcodeReaderx64.dll** to **%GOPATH%\src\github.com\dynamsoftsamples\go-barcode-reader\dbr\bin**
4. Change directory to **%GOPATH%** and compile the package with ``go install github.com\dynamsoftsamples\go-barcode-reader\dbr``. Alternatively, you can change directory to **%GOPATH%\src\github.com\dynamsoftsamples\go-barcode-reader\dbr** and compile the package with ``go install``.
5. A package **dbr.a** will be generated at **%GOPATH%\pkg\windows_amd64\github.com\dynamsoftsamples\go-barcode-reader**.
6. Create **%GOPATH%\github.com\dynamsoftsamples\go-barcode-reader\BarcodeReader\BarcodeReader.go** .

    ```go
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

    ```
7. Copy **DynamsoftBarcodeReaderx64.dll** to **%GOPATH%\bin**.
8. Change directory to **%GOPATH%** and run ``go install github.com\dynamsoftsamples\go-barcode-reader\BarcodeReader``.
9. Read a barcode image with ``%GOPATH%bin\BarcodeReader.exe <barcode image>``

Blog
----
[How to Use SWIG to Link Windows DLL with Golang][5]

[1]:http://sourceforge.net/projects/mingw-w64/
[2]:http://www.swig.org/download.html
[3]:https://golang.org/
[4]:http://www.dynamsoft.com/Downloads/Dynamic-Barcode-Reader-Download.aspx
[5]:http://www.codepool.biz/swig-link-windows-dll-golang.html
