#include "Logger.h"
#include <spdlog/sinks/stdout_color_sinks.h>

namespace Snow {
    std::shared_ptr<spdlog::logger> Logger::sLogger{ nullptr };

    b8 Logger::Initialize() {
        if (sLogger) {
            LOG_WARN("Logger already initialized");
            return false;
        }

        spdlog::sink_ptr sink{ std::make_shared<spdlog::sinks::stdout_color_sink_mt>() };
        sink->set_pattern("%^[%l | %R.%S | %v%$");
        
        sLogger = std::make_shared<spdlog::logger>("SnowEngineLogger", sink);
        sLogger->set_level(spdlog::level::trace);
        sLogger->flush_on(spdlog::level::trace);

        spdlog::register_logger(sLogger);

        LOG_INFO("Logger initialized");
        
        return true;
    }

    void Logger::Shutdown() {
        LOG_INFO("Logger shutdown");
        
        sLogger = nullptr;
    }
}