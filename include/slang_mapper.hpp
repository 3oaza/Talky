#pragma once

#include <string>
#include <unordered_map>
#include <fstream>
#include <sstream>
#include <regex>
#include "logger.hpp"

/**
 * @class SlangMapper
 * @brief High-speed local lookup for gaming slang and localized overrides.
 * 
 * Bypasses the cloud API for specific words/phrases to ensure the 
 * "gaming culture" context is preserved.
 */
class SlangMapper {
public:
    SlangMapper() = default;

    /**
     * @brief Loads slang mappings from a simple text or JSON-like format.
     * Format: "original_word,localized_slang"
     */
    void LoadDictionary(const std::string& path) {
        std::ifstream file(path);
        if (!file.is_open()) {
            TALKY_ERROR("Could not open slang dictionary: {}", path);
            return;
        }

        std::string line;
        while (std::getline(file, line)) {
            auto pos = line.find(',');
            if (pos != std::string::npos) {
                std::string key = line.substr(0, pos);
                std::string val = line.substr(pos + 1);
                m_dictionary[key] = val;
            }
        }
        TALKY_INFO("Loaded {} slang overrides.", m_dictionary.size());
    }

    /**
     * @brief Replaces detected slang in the text before/after translation.
     */
    std::string Process(const std::string& text) {
        std::string result = text;
        for (const auto& [slang, override] : m_dictionary) {
            // Simple regex replacement for whole-word matching
            std::regex re("\\b" + slang + "\\b", std::regex_constants::icase);
            result = std::regex_replace(result, re, override);
        }
        return result;
    }

private:
    std::unordered_map<std::string, std::string> m_dictionary;
};
