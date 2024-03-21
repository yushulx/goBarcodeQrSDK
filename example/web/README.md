## How to Run
1. Set the license key in `main.go`:
    
    ```go
    license := "LICENSE-KEY"
    ```
    
2. Build the project:

    ```bash
    go mod tidy
    
    # Windows
    .\run_windows.ps1
    
    # Linux
    ./run_linux.sh
    
    # mac
    chmod +x run_mac.sh
    sudo ./run_mac.sh
    ```

3. Visit `http://localhost:2024` in your web browser.
    
    ![Read barcodes on Go web server](https://www.dynamsoft.com/codepool/img/2024/03/go-web-server-barcode-reader.png)