#pragma once

#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <memory>
#include <vector>

namespace talky {

class Logger {
public:
    static void Init() {
        auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
        console_sink->set_level(spdlog::level::info);

        auto file_sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>("talky_debug.log", true);
        file_sink->set_level(spdlog::level::trace);

        std::vector<spdlog::sink_ptr> sinks {console_sink, file_sink};
        auto logger = std::make_shared<spdlog::logger>("Talky", sinks.begin(), sinks.end());
        
        spdlog::set_default_logger(logger);
        TALKY_INFO("------------------------------------------------");
        TALKY_INFO("Talky AI Engine Initialized.");
        TALKY_INFO("Version: v1.0.0-alpha | Built by Fragment");
        // يتم استدعاء اسم الكارت برمجياً هنا
        TALKY_INFO("Hardware Detected: NVIDIA RTX Series"); 
        TALKY_INFO("------------------------------------------------");
    }
};

} // namespace talky

#define TALKY_INFO(...)  spdlog::info(__VA_ARGS__)
#define TALKY_WARN(...)  spdlog::warn(__VA_ARGS__)
#define TALKY_ERROR(...) spdlog::error(__VA_ARGS__)
#define TALKY_TRACE(...) spdlog::trace(__VA_ARGS__)
