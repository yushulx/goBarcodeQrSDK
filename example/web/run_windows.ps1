$originalPath = $env:PATH

# Get the GOPATH
$GOPATH = $(go env GOPATH)

# Find the path to the shared libraries
$PACKAGE_PATH = Get-ChildItem -Path "$GOPATH\pkg\mod\github.com\yushulx" -Directory | Sort-Object LastWriteTime -Descending | Select-Object -First 1 -ExpandProperty FullName
Write-Host "PACKAGE_PATH set to $PACKAGE_PATH"
$LIBRARY_PATH = "$PACKAGE_PATH\lib\windows"

Write-Host "LIBRARY_PATH set to $LIBRARY_PATH"
# Update PATH to include the assembly path
$env:PATH = "$LIBRARY_PATH;" + $env:PATH
# Write-Host "PATH set to $($env:PATH)"

# Run your Go application
go run main.go 

$env:PATH = $originalPath
