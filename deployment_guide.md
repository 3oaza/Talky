# Talky Deployment & Distribution Guide

To create a standalone installer that runs on any modern Windows machine (NVIDIA GPU recommended), follow these steps.

## 1. Release Folder Structure
Your distribution folder should look like this after the build:

```text
Talky/
├── bin/
│   ├── Talky.exe             # The Main Binary
│   ├── Qt6Core.dll           # Qt Runtime
│   ├── onnxruntime.dll       # ONNX Engine
│   ├── onnxruntime_providers_cuda.dll # GPU Support
│   └── cublas64_11.dll       # CUDA Runtime (if not static)
├── models/
│   ├── whisper_medium.bin    # STT Model
│   └── voices/
│       ├── en_US.onnx        # Piper English
│       └── jp_JP.onnx        # Piper Japanese
├── resources/
│   └── main.qml              # Compiled QML (or embedded in binary)
└── talky_debug.log           # Generated on first run
```

## 2. Bundling AI Models Efficiently
- **Quantization**: Always use **int8** or **fp16** quantized models (.bin/.onnx) to keep the installer under 500MB.
- **Embedded Resources**: For the smallest footprint, use Qt Resource Files (.qrc) to bundle the QML and icons directly into the `Talky.exe` binary.
- **Lazy Loading**: Do not download all models with the installer. Bundle the "Universal English" model and provide an "In-App Download" feature for other languages to keep the initial .exe small.

## 3. Professional Installer (Inno Setup)
Use [Inno Setup](https://jrsoftware.org/isinfo.php) to create the `.exe`. 

### Key Script Features:
- **DirectX/CUDA Check**: Add a Pascal script to the installer to check if the user has an NVIDIA GPU and driver version > 520.
- **Virtual Audio Cable Integration**:
    Include the `VBCABLE_Setup_x64.exe` in your installer and run it silently if the registry key `HKEY_LOCAL_MACHINE\SOFTWARE\VB-Audio\Cable` is missing.
    ```pascal
    [Run]
    Filename: "{app}\drivers\VBCABLE_Setup_x64.exe"; Parameters: "/silent"; Check: NeedVBCable()
    ```

## 4. Final Optimization Checklist
- [ ] **Strip Symbols**: Use `strip` or MSVC `/DEBUG:NONE` in production.
- [ ] **Static Linking**: Try to statically link `spdlog` and `Eigen` to reduce file count.
- [ ] **DirectML Fallback**: If the user lacks an NVIDIA GPU, ensure the `TTSManager` falls back to **DirectML** (Windows native GPU API) for AMD/Intel users.
