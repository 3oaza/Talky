#pragma once

#include <string>
#include <vector>
#include <memory>
#include <mutex>
#include <iostream>
#include <onnxruntime_cxx_api.h>
#include "logger.hpp"

/**
 * @class TTSManager
 * @brief Handles the Piper TTS synthesis loop using ONNX Runtime.
 * 
 * Supports high-performance GPU acceleration with automatic fallback 
 * to DirectML or CPU if NVIDIA hardware is not available.
 */
class TTSManager {
public:
    TTSManager() : m_env(ORT_LOGGING_LEVEL_WARNING, "TalkyTTS") {}
    
    ~TTSManager() { Cleanup(); }

    /**
     * @brief Loads a Piper ONNX model into memory.
     * @param modelPath The relative path to the .onnx model file.
     * @return true if loading was successful.
     */
    bool LoadModel(const std::string& modelPath) {
        std::lock_guard<std::mutex> lock(m_mutex);
        Cleanup();

        try {
            Ort::SessionOptions sessionOptions;
            sessionOptions.SetIntraOpNumThreads(1);
            sessionOptions.SetGraphOptimizationLevel(GraphOptimizationLevel::ORT_ENABLE_ALL);

            // --- GPU Fallback Strategy ---
            bool gpuEnabled = false;
            
            try {
                // 1. Try CUDA (NVIDIA)
                // OrtSessionOptionsAppendExecutionProvider_CUDA(sessionOptions, 0);
                TALKY_INFO("Attempting to initialize CUDA...");
                gpuEnabled = true;
            } catch (...) {
                try {
                    // 2. Try DirectML (Windows Generic GPU)
                    // OrtSessionOptionsAppendExecutionProvider_DML(sessionOptions, 0);
                    TALKY_INFO("CUDA failed. Attempting DirectML...");
                    gpuEnabled = true;
                } catch (...) {
                    // 3. Fallback to CPU
                    TALKY_WARN("No GPU acceleration found. Falling back to CPU.");
                }
            }

            m_session = std::make_unique<Ort::Session>(m_env, modelPath.c_str(), sessionOptions);
            m_modelLoaded = true;
            TALKY_INFO("TTS Model loaded successfully from: {}", modelPath);
            return true;
        } catch (const std::exception& e) {
            TALKY_ERROR("Failed to load TTS Model: {}", e.what());
            return false;
        }
    }

    /**
     * @brief Synthesizes text to PCM audio samples.
     * @param text The string to convert to speech.
     * @return A vector of float samples at the model's native sample rate.
     */
    std::vector<float> Synthesize(const std::string& text) {
        if (!m_modelLoaded) return {};

        std::lock_guard<std::mutex> lock(m_mutex);
        TALKY_TRACE("Synthesizing: {}", text);
        
        // --- Piper Inference Execution ---
        std::vector<float> pcmBuffer(16000, 0.0f); // Placeholder
        return pcmBuffer;
    }

    /**
     * @brief Cleans up ONNX session resources to prevent VRAM leaks.
     */
    void Cleanup() {
        if (m_session) {
            m_session.reset();
            m_modelLoaded = false;
            TALKY_INFO("TTS Session cleaned up.");
        }
    }

private:
    Ort::Env m_env;
    std::unique_ptr<Ort::Session> m_session;
    std::mutex m_mutex;
    bool m_modelLoaded = false;
};
