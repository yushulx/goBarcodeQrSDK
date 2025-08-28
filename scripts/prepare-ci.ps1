# Script to prepare examples for CI (removes replace directives)
# This ensures CI tests the actual published module

Write-Host "Preparing examples for CI testing..." -ForegroundColor Cyan

# Function to remove replace directive from go.mod
function Remove-Replace {
    param($FilePath)
    
    if (Test-Path $FilePath) {
        Write-Host "  Processing $FilePath" -ForegroundColor Yellow
        
        # Read file content
        $content = Get-Content $FilePath
        
        # Filter out replace directive lines
        $newContent = $content | Where-Object { $_ -notmatch '^replace.*=> \.\.\/' }
        
        # Write back to file
        $newContent | Set-Content $FilePath
        
        Write-Host "  Removed replace directive from $FilePath" -ForegroundColor Green
    }
}

# Process example directories
if (Test-Path "example/command-line") {
    Remove-Replace "example/command-line/go.mod"
}

if (Test-Path "example/web") {
    Remove-Replace "example/web/go.mod"
}

Write-Host "Examples prepared for CI testing" -ForegroundColor Green
Write-Host "Examples will now use the published module from GitHub" -ForegroundColor Cyan
