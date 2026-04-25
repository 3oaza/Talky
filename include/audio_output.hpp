#pragma once

#include <windows.h>
#include <mmdeviceapi.h>
#include <audioclient.h>
#include <vector>
#include <queue>
#include <mutex>

namespace talky {

/**
 * @brief Handles rendering PCM audio to the Virtual Audio Cable (VAC).
 */
class AudioOutput {
public:
    AudioOutput() = default;
    ~AudioOutput() { Stop(); }

    bool Initialize(const wchar_t* targetDeviceName = L"CABLE Input") {
        HRESULT hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), nullptr, CLSCTX_ALL, 
                                      __uuidof(IMMDeviceEnumerator), (void**)&m_enumerator);
        
        // Find the VAC device by name
        IMMDeviceCollection* pDevices;
        m_enumerator->EnumAudioEndpoints(eRender, DEVICE_STATE_ACTIVE, &pDevices);
        
        UINT count;
        pDevices->GetCount(&count);
        for (UINT i = 0; i < count; i++) {
            IMMDevice* pDevice;
            pDevices->Item(i, &pDevice);
            
            IPropertyStore* pProps;
            pDevice->OpenPropertyStore(STGM_READ, &pProps);
            PROPVARIANT varName;
            pProps->GetValue(PKEY_Device_FriendlyName, &varName);
            
            if (wcsstr(varName.pwszVal, targetDeviceName)) {
                m_device = pDevice;
                break;
            }
        }

        if (!m_device) return false;

        m_device->Activate(__uuidof(IAudioClient), CLSCTX_ALL, nullptr, (void**)&m_audioClient);
        m_audioClient->GetMixFormat(&m_pwfx); // Usually 48kHz
        
        m_audioClient->Initialize(AUDCLNT_SHAREMODE_SHARED, 0, 1000000, 0, m_pwfx, nullptr);
        m_audioClient->GetService(__uuidof(IAudioRenderClient), (void**)&m_renderClient);
        
        return true;
    }

    /**
     * @brief Pushes Piper PCM (e.g., 16kHz) to the 48kHz VAC buffer.
     */
    void PlayAudio(const std::vector<float>& samples) {
        std::lock_guard<std::mutex> lock(m_queueMutex);
        
        // Simple Linear Resampler (16kHz -> 48kHz)
        for (size_t i = 0; i < samples.size(); ++i) {
            m_playQueue.push(samples[i]);
            m_playQueue.push(samples[i]); // Naive upsampling
            m_playQueue.push(samples[i]);
        }
    }

    void Start() {
        m_audioClient->Start();
        m_renderThread = std::jthread([this](std::stop_token st) { RenderLoop(st); });
    }

    void Stop() {
        if (m_audioClient) m_audioClient->Stop();
    }

private:
    void RenderLoop(std::stop_token st) {
        while (!st.stop_requested()) {
            UINT32 padding;
            m_audioClient->GetCurrentPadding(&padding);
            UINT32 numFramesRequested = m_pwfx->nSamplesPerSec / 100 - padding;

            if (numFramesRequested > 0) {
                BYTE* pData;
                m_renderClient->GetBuffer(numFramesRequested, &pData);
                
                float* pFloatData = reinterpret_cast<float*>(pData);
                {
                    std::lock_guard<std::mutex> lock(m_queueMutex);
                    for (UINT32 i = 0; i < numFramesRequested; i++) {
                        if (!m_playQueue.empty()) {
                            pFloatData[i] = m_playQueue.front();
                            m_playQueue.pop();
                        } else {
                            pFloatData[i] = 0.0f; // Silence
                        }
                    }
                }
                
                m_renderClient->ReleaseBuffer(numFramesRequested, 0);
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    }

    IMMDeviceEnumerator* m_enumerator = nullptr;
    IMMDevice* m_device = nullptr;
    IAudioClient* m_audioClient = nullptr;
    IAudioRenderClient* m_renderClient = nullptr;
    WAVEFORMATEX* m_pwfx = nullptr;
    
    std::queue<float> m_playQueue;
    std::mutex m_queueMutex;
    std::jthread m_renderThread;
};

} // namespace talky
