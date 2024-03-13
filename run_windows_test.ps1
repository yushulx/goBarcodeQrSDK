# Save the current PATH to restore it later
$originalPath = $env:PATH

# Define the path to your DLLs
$dllPath = "lib\windows"

# Prepend the DLL path to the PATH environment variable
$env:PATH = "$dllPath;$originalPath"

# Run 'go test'
go test

# Restore the original PATH
$env:PATH = $originalPath
