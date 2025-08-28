# Local CI Check Script for Windows
# This script mimics what GitHub Actions will do, so you can test locally

Write-Host "🔍 Local CI Check for goBarcodeQrSDK" -ForegroundColor Cyan
Write-Host "==================================" -ForegroundColor Cyan

# Check if we're in the right directory
if (-not (Test-Path "go.mod")) {
    Write-Host "❌ Error: Please run this script from the project root directory" -ForegroundColor Red
    exit 1
}

Write-Host "📦 Checking Go module..." -ForegroundColor Yellow
go mod tidy
if ($LASTEXITCODE -ne 0) { Write-Host "❌ go mod tidy failed" -ForegroundColor Red; exit 1 }

go mod verify
if ($LASTEXITCODE -ne 0) { Write-Host "❌ go mod verify failed" -ForegroundColor Red; exit 1 }

Write-Host "🔧 Building bridge library..." -ForegroundColor Yellow
if (Test-Path "build") {
    Remove-Item -Recurse -Force "build"
}
mkdir build | Out-Null
Set-Location build

cmake .. -G "Visual Studio 17 2022" -A x64
if ($LASTEXITCODE -ne 0) { Write-Host "❌ CMake configure failed" -ForegroundColor Red; exit 1 }

cmake --build . --config Release
if ($LASTEXITCODE -ne 0) { Write-Host "❌ CMake build failed" -ForegroundColor Red; exit 1 }

Set-Location ..

Write-Host "🧪 Running tests..." -ForegroundColor Yellow
$env:CGO_ENABLED = "1"
go test -v
if ($LASTEXITCODE -ne 0) { Write-Host "❌ Tests failed" -ForegroundColor Red; exit 1 }

Write-Host "🔨 Building examples..." -ForegroundColor Yellow
Set-Location example/command-line
go mod tidy
if ($LASTEXITCODE -ne 0) { Write-Host "❌ Command-line example go mod tidy failed" -ForegroundColor Red; exit 1 }

go build .
if ($LASTEXITCODE -ne 0) { Write-Host "❌ Command-line example build failed" -ForegroundColor Red; exit 1 }
Write-Host "✅ Command-line example built successfully" -ForegroundColor Green

Set-Location ../web
go mod tidy
if ($LASTEXITCODE -ne 0) { Write-Host "❌ Web example go mod tidy failed" -ForegroundColor Red; exit 1 }

go build .
if ($LASTEXITCODE -ne 0) { Write-Host "❌ Web example build failed" -ForegroundColor Red; exit 1 }
Write-Host "✅ Web example built successfully" -ForegroundColor Green

Set-Location ../..

Write-Host ""
Write-Host "🎉 All checks passed! Your changes should work in CI." -ForegroundColor Green
Write-Host "💡 To create a release, run: git tag v2.x.x && git push origin v2.x.x" -ForegroundColor Cyan
