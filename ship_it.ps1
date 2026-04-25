# ship_it.ps1 - Talky Production Bundler
$ErrorActionPreference = "Stop"

$BUILD_DIR = "./build/Release"
$DIST_DIR = "./dist"
$MODELS_DIR = "./models"

Write-Host "[*] Creating Distribution Bundle..." -ForegroundColor Cyan

# 1. Clean and Create Folders
if (Test-Path $DIST_DIR) { Remove-Item $DIST_DIR -Recurse -Force }
New-Item -ItemType Directory -Path "$DIST_DIR/bin" -Force
New-Item -ItemType Directory -Path "$DIST_DIR/models" -Force

# 2. Copy the Binary
Copy-Item "$BUILD_DIR/Talky.exe" "$DIST_DIR/bin/"

# 3. Run windeployqt to gather Qt DLLs
Write-Host "[*] Gathering Qt Dependencies..."
windeployqt --release --qmldir ./ui "$DIST_DIR/bin/Talky.exe"

# 4. Copy AI Models
Write-Host "[*] Copying AI Models..."
Copy-Item "$MODELS_DIR/*" "$DIST_DIR/models/" -Recurse

# 5. Copy ONNX Runtime DLLs (Assuming vcpkg location)
# This varies by install, but usually located in vcpkg_installed/x64-windows/bin
$VCPKG_BIN = "./vcpkg_installed/x64-windows/bin"
Copy-Item "$VCPKG_BIN/onnxruntime*.dll" "$DIST_DIR/bin/"

Write-Host "[+] Bundle Complete: $DIST_DIR" -ForegroundColor Green
