#pragma once

#include <QObject>
#include <QString>
#include <QTimer>
#include <QCoreApplication>
#include <QtConcurrent>
#include "audio_engine.hpp"
#include "audio_output.hpp"
#include "translation_manager.hpp"
#include "tts_manager.hpp"
#include "logger.hpp"

/**
 * @class AppController
 * @brief The central orchestrator for the Talky application.
 * 
 * Exposes core application state to QML via reactive properties and
 * handles the lifecycle of the audio and AI inference engines.
 */
class AppController : public QObject {
    Q_OBJECT
    Q_PROPERTY(float audioLevel READ audioLevel NOTIFY audioLevelChanged)
    Q_PROPERTY(bool isLoading READ isLoading NOTIFY isLoadingChanged)
    Q_PROPERTY(bool isSpeaking READ isSpeaking NOTIFY isSpeakingChanged)
    Q_PROPERTY(QString statusText READ statusText NOTIFY statusTextChanged)
    Q_PROPERTY(QString lastTranslation READ lastTranslation NOTIFY lastTranslationChanged)
    Q_PROPERTY(bool unfilteredMode READ unfilteredMode WRITE setUnfilteredMode NOTIFY unfilteredModeChanged)

public:
    explicit AppController(QObject *parent = nullptr) : QObject(parent) {
        m_uiTimer.setInterval(16); // 60fps
        connect(&m_uiTimer, &QTimer::timeout, this, &AppController::updateUI);
        connect(&m_transManager, &TranslationManager::translationFinished, this, &AppController::handleTranslation);
    }

    // --- Properties ---
    float audioLevel() const { return m_audioLevel.load(); }
    bool isLoading() const { return m_isLoading; }
    bool isSpeaking() const { return m_isSpeaking; }
    bool unfilteredMode() const { return m_unfilteredMode; }
    QString statusText() const { return m_statusText; }
    QString lastTranslation() const { return m_lastTranslation; }

    void setUnfilteredMode(bool mode) {
        if (m_unfilteredMode != mode) {
            m_unfilteredMode = mode;
            TALKY_INFO("Unfiltered Mode toggled: {}", mode ? "ON" : "OFF");
            emit unfilteredModeChanged();
        }
    }

    /**
     * @brief Reactively updates the target translation language.
     * @param langCode The language identifier (e.g., "jp").
     */
    Q_INVOKABLE void setTargetLanguage(const QString& langCode) {
        TALKY_INFO("Target language changed to: {}", langCode.toStdString());
        // Handle model path update here
    }

    /**
     * @brief Bootstraps the application services and loads models asynchronously.
     */
    void start() {
        m_isLoading = true;
        emit isLoadingChanged();

        QtConcurrent::run([this]() {
            TALKY_INFO("Starting Talky Core Services...");
            
            // Use Relative Paths based on the executable location
            QString appPath = QCoreApplication::applicationDirPath();
            
            // 1. Load Local TTS Model
            QString ttsModelPath = appPath + "/models/voice_model.onnx";
            m_tts.LoadModel(ttsModelPath.toStdString());

            // 2. Load Local Translation Model (GGUF)
            QString llmModelPath = appPath + "/models/translation_model.gguf";
            m_transManager.Initialize(llmModelPath.toStdString());

            m_engine.Initialize([this](const std::vector<float>& buffer) {
                this->handleAudioBuffer(buffer);
            });
            
            m_engine.Start();
            m_output.Initialize(L"CABLE Input");
            m_output.Start();
            
            m_isLoading = false;
            m_statusText = "System Ready";
            emit isLoadingChanged();
            emit statusTextChanged();
            m_uiTimer.start();
        });
    }

signals:
    void audioLevelChanged();
    void isLoadingChanged();
    void isSpeakingChanged();
    void unfilteredModeChanged();
    void statusTextChanged();
    void lastTranslationChanged();

private:
    void handleTranslation(const QString& text) {
        m_lastTranslation = text;
        emit lastTranslationChanged();

        QtConcurrent::run([this, text]() {
            m_isSpeaking = true;
            emit isSpeakingChanged();
            auto pcm = m_tts.Synthesize(text.toStdString());
            m_output.PlayAudio(pcm);
            m_isSpeaking = false;
            emit isSpeakingChanged();
        });
    }

    void handleAudioBuffer(const std::vector<float>& buffer) {
        float maxVal = 0.0f;
        for (float s : buffer) maxVal = std::max(maxVal, std::abs(s));
        m_audioLevel.store(maxVal);
        
        // Example trigger:
        // m_transManager.Translate(captured_text, m_unfilteredMode);
    }

    void updateUI() {
        emit audioLevelChanged();
    }

    talky::AudioEngine m_engine;
    talky::AudioOutput m_output;
    TranslationManager m_transManager;
    TTSManager m_tts;
    QTimer m_uiTimer;
    
    std::atomic<float> m_audioLevel{0.0f};
    bool m_isLoading = false;
    bool m_isSpeaking = false;
    bool m_unfilteredMode = false;
    QString m_statusText = "Initializing...";
    QString m_lastTranslation = "";
};
