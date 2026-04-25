#pragma once

#include <QObject>
#include <QString>
#include <string>
#include "local_llm.hpp"
#include "slang_mapper.hpp"
#include "logger.hpp"

/**
 * @brief 100% Offline Translation Manager.
 * Uses llama.cpp for local inference and SlangMapper for overrides.
 */
class TranslationManager : public QObject {
    Q_OBJECT
public:
    explicit TranslationManager(QObject *parent = nullptr) : QObject(parent) {
        m_slangMapper.LoadDictionary("resources/slang_overrides.csv");
    }

    /**
     * @brief Boots the local AI engine.
     */
    void Initialize(const std::string& modelPath) {
        m_localEngine.LoadModel(modelPath);
    }

    /**
     * @brief High-speed local translation pipeline.
     */
    void Translate(const std::string& input, bool isUnfiltered) {
        // 1. Apply Local Overrides first
        std::string processedInput = m_slangMapper.Process(input);

        // 2. Local AI Inference
        std::string prompt = "Translate this gaming chat naturally: ";
        if (isUnfiltered) prompt += "Keep all slang and profanity. ";
        
        std::string translated = m_localEngine.Translate(prompt, processedInput);

        emit translationFinished(QString::fromStdString(translated));
    }

signals:
    void translationFinished(const QString& text);

private:
    LocalLLMEngine m_localEngine;
    SlangMapper m_slangMapper;
};
