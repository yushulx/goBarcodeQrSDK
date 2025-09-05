# Build Script for Windows
# Builds the bridge library and Go modules

param(
    [string]$Configuration = "Release",
    [switch]$Clean = $false,
    [switch]$Verbose = $false
)

$ErrorActionPreference = "Stop"

function Write-Status($Message) {
    Write-Host ">> $Message" -ForegroundColor Cyan
}

function Write-Success($Message) {
    Write-Host "SUCCESS: $Message" -ForegroundColor Green
}

function Write-Error($Message) {
    Write-Host "ERROR: $Message" -ForegroundColor Red
}

Write-Host "Building goBarcodeQrSDK for Windows" -ForegroundColor Yellow
Write-Host "===================================" -ForegroundColor Yellow

# Check prerequisites
Write-Status "Checking prerequisites..."

if (-not (Get-Command cmake -ErrorAction SilentlyContinue)) {
    Write-Error "CMake not found. Please install CMake and add it to PATH."
    exit 1
}

if (-not (Get-Command go -ErrorAction SilentlyContinue)) {
    Write-Error "Go not found. Please install Go and add it to PATH."
    exit 1
}

# Check if we're in the right directory
if (-not (Test-Path "go.mod")) {
    Write-Error "Please run this script from the project root directory"
    exit 1
}

# Clean build directory if requested
if ($Clean -and (Test-Path "build")) {
    Write-Status "Cleaning build directory..."
    Remove-Item -Recurse -Force "build"
}

# Create build directory
if (-not (Test-Path "build")) {
    Write-Status "Creating build directory..."
    New-Item -ItemType Directory -Path "build" | Out-Null
}

# Configure CMake
Write-Status "Configuring CMake..."
Push-Location build

$cmakeArgs = @(
    ".."
    "-G", "Visual Studio 17 2022"
    "-A", "x64"
)

if ($Verbose) {
    $cmakeArgs += "--verbose"
}

& cmake @cmakeArgs
if ($LASTEXITCODE -ne 0) {
    Write-Error "CMake configuration failed"
    Pop-Location
    exit 1
}

# Build the bridge library
Write-Status "Building bridge library ($Configuration)..."
$buildArgs = @(
    "."
    "--config", $Configuration
)

if ($Verbose) {
    $buildArgs += "--verbose"
}

& cmake --build @buildArgs
if ($LASTEXITCODE -ne 0) {
    Write-Error "Bridge library build failed"
    Pop-Location
    exit 1
}

Pop-Location

# Verify bridge library was created
$bridgeDll = "dcv\lib\win\Release\bridge.dll"
if (-not (Test-Path $bridgeDll)) {
    Write-Error "Bridge library not found at $bridgeDll"
    exit 1
}

Write-Success "Bridge library built successfully"

# Set CGO environment
$env:CGO_ENABLED = "1"

# Add DLL paths to PATH for testing
$currentPath = $env:PATH
$dllPath1 = Join-Path (Get-Location) "dcv\lib\win"
$dllPath2 = Join-Path (Get-Location) "dcv\lib\win\Release"
$env:PATH = "$currentPath;$dllPath1;$dllPath2"

# Build and test Go module
Write-Status "Testing Go module..."
go mod tidy
if ($LASTEXITCODE -ne 0) {
    Write-Error "go mod tidy failed"
    exit 1
}

go test -v
if ($LASTEXITCODE -ne 0) {
    Write-Error "Go tests failed"
    exit 1
}

Write-Success "Go module tests passed"

# Build examples
Write-Status "Building examples..."

# Command-line example
Push-Location "example\command-line"
go mod tidy
if ($LASTEXITCODE -ne 0) {
    Write-Error "Command-line example go mod tidy failed"
    Pop-Location
    exit 1
}

go build -o "barcode-reader.exe" .
if ($LASTEXITCODE -ne 0) {
    Write-Error "Command-line example build failed"
    Pop-Location
    exit 1
}
Write-Success "Command-line example built"
Pop-Location

# Web example
Push-Location "example\web"
go mod tidy
if ($LASTEXITCODE -ne 0) {
    Write-Error "Web example go mod tidy failed"
    Pop-Location
    exit 1
}

go build -o "web-server.exe" .
if ($LASTEXITCODE -ne 0) {
    Write-Error "Web example build failed"
    Pop-Location
    exit 1
}
Write-Success "Web example built"
Pop-Location

# Desktop example (simple version)
if (Test-Path "example\desktop\main_simple.go") {
    Push-Location "example\desktop"
    
    # Use simple go.mod without OpenCV dependencies
    if (Test-Path "go_simple.mod") {
        Copy-Item "go_simple.mod" "go.mod" -Force
    }
    
    go mod tidy
    if ($LASTEXITCODE -eq 0) {
        go build -o "barcode-scanner-simple.exe" "main_simple.go"
        if ($LASTEXITCODE -eq 0) {
            Write-Success "Desktop simple example built"
        }
        else {
            Write-Host "WARNING: Desktop simple example build failed (non-critical)" -ForegroundColor Yellow
        }
    }
    else {
        Write-Host "WARNING: Desktop simple example go mod tidy failed (non-critical)" -ForegroundColor Yellow
    }
    Pop-Location
}

Write-Host ""
Write-Success "Build completed successfully!"
Write-Host "Built artifacts:" -ForegroundColor Cyan
Write-Host "  - Bridge library: dcv\lib\win\Release\bridge.dll" -ForegroundColor White
Write-Host "  - Command-line: example\command-line\barcode-reader.exe" -ForegroundColor White
Write-Host "  - Web server: example\web\web-server.exe" -ForegroundColor White
if (Test-Path "example\desktop\barcode-scanner-simple.exe") {
    Write-Host "  - Desktop app: example\desktop\barcode-scanner-simple.exe" -ForegroundColor White
}
Write-Host ""
Write-Host "Run examples with:" -ForegroundColor Cyan
Write-Host "  .\example\command-line\barcode-reader.exe <image_file>" -ForegroundColor White
Write-Host "  .\example\web\web-server.exe" -ForegroundColor White
Write-Host "  .\example\desktop\barcode-scanner-simple.exe <image_file>" -ForegroundColor White
