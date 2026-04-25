#pragma once

#include <windows.h>
#include <mmdeviceapi.h>
#include <audioclient.h>
#include <vector>
#include <atomic>
#include <thread>
#include <functional>
#include <iostream>

namespace talky {

/**
 * @brief High-performance WASAPI Audio Engine.
 * Handles low-latency capture and passes buffers to the inference engine.
 */
class AudioEngine {
public:
    using AudioCallback = std::function<void(const std::vector<float>&)>;

    AudioEngine() = default;
    ~AudioEngine() { Stop(); }

    bool Initialize(AudioCallback callback) {
        m_callback = callback;
        
        HRESULT hr = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
        if (FAILED(hr)) return false;

        hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), nullptr, CLSCTX_ALL, 
                              __uuidof(IMMDeviceEnumerator), (void**)&m_enumerator);
        if (FAILED(hr)) return false;

        hr = m_enumerator->GetDefaultAudioEndpoint(eCapture, eConsole, &m_device);
        if (FAILED(hr)) return false;

        hr = m_device->Activate(__uuidof(IAudioClient), CLSCTX_ALL, nullptr, (void**)&m_audioClient);
        if (FAILED(hr)) return false;

        hr = m_audioClient->GetMixFormat(&m_pwfx);
        if (FAILED(hr)) return false;

        // Initialize for low-latency shared mode
        hr = m_audioClient->Initialize(AUDCLNT_SHAREMODE_SHARED, 0, 10000000, 0, m_pwfx, nullptr);
        if (FAILED(hr)) return false;

        hr = m_audioClient->GetService(__uuidof(IAudioCaptureClient), (void**)&m_captureClient);
        return SUCCEEDED(hr);
    }

    void Start() {
        m_running = true;
        m_captureThread = std::jthread([this](std::stop_token st) { CaptureLoop(st); });
    }

    void Stop() {
        m_running = false;
        if (m_audioClient) m_audioClient->Stop();
    }

private:
    void CaptureLoop(std::stop_token st) {
        HRESULT hr = m_audioClient->Start();
        if (FAILED(hr)) return;

        UINT32 packetSize = 0;
        while (!st.stop_requested() && m_running) {
            hr = m_captureClient->GetNextPacketSize(&packetSize);
            if (packetSize == 0) {
                std::this_thread::sleep_for(std::chrono::milliseconds(1));
                continue;
            }

            BYTE* pData;
            UINT32 numFramesAvailable;
            DWORD flags;

            hr = m_captureClient->GetBuffer(&pData, &numFramesAvailable, &flags, nullptr, nullptr);
            if (SUCCEEDED(hr)) {
                // Convert raw bytes to float samples (assuming IEEE Float format from WASAPI)
                float* pFloatData = reinterpret_cast<float*>(pData);
                std::vector<float> buffer(pFloatData, pFloatData + numFramesAvailable);
                
                if (m_callback) m_callback(buffer);

                m_captureClient->ReleaseBuffer(numFramesAvailable);
            }
        }
    }

    IMMDeviceEnumerator* m_enumerator = nullptr;
    IMMDevice* m_device = nullptr;
    IAudioClient* m_audioClient = nullptr;
    IAudioCaptureClient* m_captureClient = nullptr;
    WAVEFORMATEX* m_pwfx = nullptr;
    
    std::atomic<bool> m_running{false};
    std::jthread m_captureThread;
    AudioCallback m_callback;
};

} // namespace talky
