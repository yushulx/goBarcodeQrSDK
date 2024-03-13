# Get the GOPATH
# $GOPATH = $(go env GOPATH)

# Find the path to the shared libraries
# $PACKAGE_PATH = Get-ChildItem -Path "$GOPATH\pkg\mod\github.com\yushulx" -Directory | Sort-Object LastWriteTime -Descending | Select-Object -First 1 -ExpandProperty FullName
$ASSEMBLY_PATH = "../../lib/windows"
Write-Host "ASSEMBLY_PATH set to $ASSEMBLY_PATH"
# Update PATH to include the assembly path
$env:PATH = "$ASSEMBLY_PATH;" + $env:PATH
# Write-Host "PATH set to $($env:PATH)"

# Run your Go application
go run test.go test.png
