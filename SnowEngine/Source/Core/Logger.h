#pragma once
#include <assert.h>
#include <spdlog/spdlog.h>
#include "Common.h"

namespace Snow {
    enum class LogLevel : u32 {
        Trace,
        Info,
        Warn,
        Error,
        Critical
    };
    
    struct LogMessage {
        std::string Message;
        u32 Line;
        std::string File;
        u32 RepeatCount{ 1 };
    };

    class Logger {
    public:
        static b8 Initialize();
        static void Shutdown();

        template<typename... Args>
        static void Log(const LogLevel& level, u32 line, const std::string& file, const std::string& message, Args&&... args) {
            static std::string prefix{ "{} | {}] " };
            std::string out = prefix + message;

            switch (level) {
            case LogLevel::Trace:
                sLogger->trace(out, file, line, std::forward<Args>(args)...);
                break;
            case LogLevel::Info:
                sLogger->info(out, file, line, std::forward<Args>(args)...);
                break;
            case LogLevel::Warn:
                sLogger->warn(out, file, line, std::forward<Args>(args)...);
                break;
            case LogLevel::Error:
                sLogger->error(out, file, line, std::forward<Args>(args)...);
                break;
            case LogLevel::Critical:
                sLogger->critical(out, file, line, std::forward<Args>(args)...);
                break;
            }
        };
        
    private:
        Logger() = delete;

        static std::shared_ptr<spdlog::logger> sLogger;
    };
}


#define LOG_TRACE(...) Snow::Logger::Log(Snow::LogLevel::Trace, __LINE__, __FILE__, __VA_ARGS__)
#define LOG_INFO(...) Snow::Logger::Log(Snow::LogLevel::Info, __LINE__, __FILE__, __VA_ARGS__)
#define LOG_WARN(...) Snow::Logger::Log(Snow::LogLevel::Warn, __LINE__, __FILE__, __VA_ARGS__)
#define LOG_ERROR(...) Snow::Logger::Log(Snow::LogLevel::Error, __LINE__, __FILE__, __VA_ARGS__)
#define LOG_CRITICAL(...) Snow::Logger::Log(Snow::LogLevel::Critical, __LINE__, __FILE__, __VA_ARGS__)
#define LOG_ASSERT(condition, ...) \
if (!(condition)) { \
    LOG_CRITICAL(__VA_ARGS__); \
    assert(0); \
}
