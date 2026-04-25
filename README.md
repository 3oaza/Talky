<p align="center">
  <img src="logo.ico" width="128" height="128" />
</p>

# 🎧 Talky - The Raw Gaming Translator (v1.0-alpha)

Talky is a **100% Offline**, C++ powered voice translator designed for competitive gamers. It translates incoming voice chat in real-time and broadcasts it through a virtual microphone—with **zero latency** and **zero censorship**.

## ✨ Key Features
- **Total Privacy**: Everything runs locally on your GPU/CPU. No data leaves your PC.
- **Gamer-Centric**: Detects and translates street slang and profanity accurately.
- **High Performance**: Powered by `llama.cpp` and `Whisper.cpp` with CUDA acceleration.
- **Latency Monitoring**: Real-time stats (ms and VRAM) to keep your game smooth.

## 🛠️ Developer Setup (Build from Source)
1. **Prerequisites**: 
   - Visual Studio 2022 (C++ Desktop workload)
   - CMake 3.25+
   - [vcpkg](https://github.com/microsoft/vcpkg)
2. **Build**:
   ```bash
   git clone https://github.com/YOUR_USERNAME/Talky-AI.git
   cd Talky-AI
   vcpkg install
   cmake -B build -DGGML_CUDA=ON
   cmake --build build --config Release
   ```

## 🚀 How to Use
1. **Models**: Download the [Llama-3-8B GGUF Model](https://huggingface.co/bartowski/Meta-Llama-3-8B-Instruct-GGUF) and place it in the `/models` folder as `translation_model.gguf`.
2. **Virtual Mic**: Install [VB-Cable](https://vb-audio.com/Cable/) and set your game's Input Device to "CABLE Output".
3. **Activate**: Launch Talky.exe and enter the Alpha Key: `TALKY-2026`.
4. **Translate**: Select your languages and toggle **Unfiltered Mode** for raw authenticity.

## ⚖️ License & Privacy
Talky is open-source. It does not collect any voice data or text.

---
*Built by Fragment*
*Empowering voices, one conversation at a time.*
