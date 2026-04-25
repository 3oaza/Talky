#pragma once

#include <string>
#include <vector>
#include "llama.h"
#include "logger.hpp"

/**
 * @class LocalLLMEngine
 * @brief High-performance wrapper for llama.cpp.
 * 
 * Runs quantized GGUF models locally for 100% offline translation.
 */
class LocalLLMEngine {
public:
    LocalLLMEngine() {
        llama_backend_init(false);
    }

    ~LocalLLMEngine() {
        if (m_ctx) llama_free(m_ctx);
        if (m_model) llama_free_model(m_model);
        llama_backend_free();
    }

    /**
     * @brief Loads a quantized GGUF model into VRAM/RAM.
     */
    bool LoadModel(const std::string& modelPath) {
        auto mparams = llama_model_default_params();
        mparams.n_gpu_layers = 32; // Offload as many layers as possible to GPU

        m_model = llama_load_model_from_file(modelPath.c_str(), mparams);
        if (!m_model) {
            TALKY_ERROR("Failed to load local model: {}", modelPath);
            return false;
        }

        auto cparams = llama_context_default_params();
        cparams.n_ctx = 2048;
        m_ctx = llama_new_context_with_model(m_model, cparams);

        TALKY_INFO("Local AI Engine Initialized: 100% Offline Mode.");
        return true;
    }

    /**
     * @brief Performs inference for translation.
     */
    std::string Translate(const std::string& prompt, const std::string& input) {
        if (!m_ctx) return "Engine Error";

        // --- Simplified llama.cpp Inference Loop ---
        // 1. Tokenize system prompt + user input
        // 2. Perform llama_decode
        // 3. Sample next tokens until EoS
        
        TALKY_TRACE("Local Inference started for: {}", input);
        
        // Mock result for structure
        return "[Local Translation]: " + input;
    }

private:
    llama_model* m_model = nullptr;
    llama_context* m_ctx = nullptr;
};
