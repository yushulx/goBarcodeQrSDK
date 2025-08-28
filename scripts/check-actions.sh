#!/bin/bash

# Script to check for deprecated GitHub Actions
echo "🔍 Checking for deprecated GitHub Actions..."

echo "📋 Scanning workflow files..."

# Check for deprecated actions
deprecated_found=false

for file in .github/workflows/*.yml; do
    if [ -f "$file" ]; then
        echo "  📄 Checking $file"
        
        # Check for deprecated artifact actions
        if grep -q "upload-artifact@v[1-3]" "$file"; then
            echo "    ❌ Found deprecated upload-artifact (v1-v3) - should use v4"
            deprecated_found=true
        fi
        
        if grep -q "download-artifact@v[1-3]" "$file"; then
            echo "    ❌ Found deprecated download-artifact (v1-v3) - should use v4"
            deprecated_found=true
        fi
        
        # Check for deprecated release actions
        if grep -q "create-release@v1" "$file"; then
            echo "    ❌ Found deprecated create-release@v1 - should use softprops/action-gh-release@v2"
            deprecated_found=true
        fi
        
        if grep -q "upload-release-asset@v1" "$file"; then
            echo "    ❌ Found deprecated upload-release-asset@v1 - should use softprops/action-gh-release@v2"
            deprecated_found=true
        fi
        
        # Check for deprecated MSBuild
        if grep -q "setup-msbuild@v1" "$file"; then
            echo "    ❌ Found deprecated setup-msbuild@v1 - should use v2"
            deprecated_found=true
        fi
        
        # Check current versions
        if grep -q "upload-artifact@v4" "$file"; then
            echo "    ✅ Using current upload-artifact@v4"
        fi
        
        if grep -q "download-artifact@v4" "$file"; then
            echo "    ✅ Using current download-artifact@v4"
        fi
        
        if grep -q "softprops/action-gh-release@v2" "$file"; then
            echo "    ✅ Using current action-gh-release@v2"
        fi
        
        if grep -q "setup-msbuild@v2" "$file"; then
            echo "    ✅ Using current setup-msbuild@v2"
        fi
    fi
done

echo ""
if [ "$deprecated_found" = true ]; then
    echo "❌ Deprecated actions found! Please update them before pushing."
    exit 1
else
    echo "✅ All GitHub Actions are using current versions!"
    echo "🚀 Safe to push to GitHub!"
fi
