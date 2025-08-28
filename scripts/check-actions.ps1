# Script to check for deprecated GitHub Actions
Write-Host "🔍 Checking for deprecated GitHub Actions..." -ForegroundColor Cyan

Write-Host "📋 Scanning workflow files..." -ForegroundColor Yellow

$deprecatedFound = $false

Get-ChildItem ".github/workflows/*.yml" | ForEach-Object {
    $file = $_.FullName
    Write-Host "  📄 Checking $($_.Name)" -ForegroundColor Gray
    
    $content = Get-Content $file -Raw
    
    # Check for deprecated artifact actions
    if ($content -match "upload-artifact@v[1-3]") {
        Write-Host "    ❌ Found deprecated upload-artifact (v1-v3) - should use v4" -ForegroundColor Red
        $deprecatedFound = $true
    }
    
    if ($content -match "download-artifact@v[1-3]") {
        Write-Host "    ❌ Found deprecated download-artifact (v1-v3) - should use v4" -ForegroundColor Red
        $deprecatedFound = $true
    }
    
    # Check for deprecated release actions
    if ($content -match "create-release@v1") {
        Write-Host "    ❌ Found deprecated create-release@v1 - should use softprops/action-gh-release@v2" -ForegroundColor Red
        $deprecatedFound = $true
    }
    
    if ($content -match "upload-release-asset@v1") {
        Write-Host "    ❌ Found deprecated upload-release-asset@v1 - should use softprops/action-gh-release@v2" -ForegroundColor Red
        $deprecatedFound = $true
    }
    
    # Check for deprecated MSBuild
    if ($content -match "setup-msbuild@v1") {
        Write-Host "    ❌ Found deprecated setup-msbuild@v1 - should use v2" -ForegroundColor Red
        $deprecatedFound = $true
    }
    
    # Check current versions
    if ($content -match "upload-artifact@v4") {
        Write-Host "    ✅ Using current upload-artifact@v4" -ForegroundColor Green
    }
    
    if ($content -match "download-artifact@v4") {
        Write-Host "    ✅ Using current download-artifact@v4" -ForegroundColor Green
    }
    
    if ($content -match "softprops/action-gh-release@v2") {
        Write-Host "    ✅ Using current action-gh-release@v2" -ForegroundColor Green
    }
    
    if ($content -match "setup-msbuild@v2") {
        Write-Host "    ✅ Using current setup-msbuild@v2" -ForegroundColor Green
    }
    
    if ($content -match "jwlawson/actions-setup-cmake@v2") {
        Write-Host "    ✅ Using actions-setup-cmake@v2" -ForegroundColor Green
    }
}

Write-Host ""
if ($deprecatedFound) {
    Write-Host "❌ Deprecated actions found! Please update them before pushing." -ForegroundColor Red
    exit 1
}
else {
    Write-Host "✅ All GitHub Actions are using current versions!" -ForegroundColor Green
    Write-Host "🚀 Safe to push to GitHub!" -ForegroundColor Cyan
}
