#include "Core/Log.h"

#include <spdlog/logger.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>

#include <array>
#include <memory>
#include <string>
#include <utility>
#include <vector>

// spdlog terms used in this file:
//   logger - the object messages are written to. It has a name (printed as "[Renderer]") and a minimum level.
//            Every accepted message is passed on to all sinks of the logger.
//   sink   - a destination where a message ends up: the console, a file, ... One sink can be used by several loggers.
namespace Abomination::Core::Log
{
    namespace
    {
        constexpr std::array AllCategories{
            LogCategory::Core,
            LogCategory::Platform,
            LogCategory::Renderer,
        };

        // [12:03:41.512] [Renderer] [warning] Message
        //  %H:%M:%S.%e - time with milliseconds, %n - logger name, %l - level, %v - the message itself.
        //  The text between %^ and %$ is colored by the level (only in the console).
        constexpr const char* MessagePattern = "[%H:%M:%S.%e] [%n] [%^%l%$] %v";

        // One logger per category; the index in the array is the numeric value of LogCategory.
        // All loggers write to the same sinks and differ only by the name printed in the message.
        // All pointers are nullptr while logging is not initialized.
        std::array<std::unique_ptr<spdlog::logger>, AllCategories.size()> loggers;

        constexpr std::string_view ConvertToString(LogCategory category) noexcept
        {
            switch (category)
            {
                case LogCategory::Core:
                    return "Core";
                case LogCategory::Platform:
                    return "Platform";
                case LogCategory::Renderer:
                    return "Renderer";
            }
            return "Unknown";
        }

        constexpr spdlog::level::level_enum ConvertToSPDLogLevel(LogLevel level) noexcept
        {
            switch (level)
            {
                case LogLevel::Trace:
                    return spdlog::level::trace;
                case LogLevel::Debug:
                    return spdlog::level::debug;
                case LogLevel::Info:
                    return spdlog::level::info;
                case LogLevel::Warning:
                    return spdlog::level::warn;
                case LogLevel::Error:
                    return spdlog::level::err;
                case LogLevel::Critical:
                    return spdlog::level::critical;
            }
            return spdlog::level::info;
        }

        // Returns nullptr while logging is not initialized.
        spdlog::logger* GetLogger(LogCategory category) noexcept
        {
            return loggers[std::to_underlying(category)].get();
        }
    }

    void Initialize(const LogSettings& settings)
    {
        // If logging is already running, stop it first and start from a clean state.
        Shutdown();

        // 1. Create the sinks (destinations). spdlog requires sinks to be held by std::shared_ptr because one sink is
        //    shared by several loggers: spdlog::sink_ptr is an alias for std::shared_ptr<spdlog::sinks::sink>.
        //    The "_mt" suffix means "multi-threaded": the sink may be used from several threads at the same time.
        std::vector<spdlog::sink_ptr> sinks;

        if (settings.needToWriteToConsole)
            sinks.push_back(std::make_shared<spdlog::sinks::stdout_color_sink_mt>()); // Console output, levels in color

        constexpr bool NeedToTruncateFile = true; // Every run starts with an empty log file
        sinks.push_back(std::make_shared<spdlog::sinks::basic_file_sink_mt>(settings.filePath.string(), NeedToTruncateFile));

        // 2. Create one logger per category. Each logger gets the category name and all sinks created above.
        for (const LogCategory category : AllCategories)
        {
            auto logger = std::make_unique<spdlog::logger>(std::string(ConvertToString(category)), sinks.begin(), sinks.end());

            // How every message line looks, see MessagePattern.
            logger->set_pattern(MessagePattern);

            // Messages below this level are discarded without being formatted or written.
            logger->set_level(ConvertToSPDLogLevel(settings.minimumLevel));

            // spdlog collects messages in a buffer and writes them to the file in batches, which is faster.
            // Warnings and more important messages are written to the file immediately instead,
            // so they are not lost if the game crashes right after them.
            logger->flush_on(spdlog::level::warn);

            loggers[std::to_underlying(category)] = std::move(logger);
        }
    }

    void Shutdown()
    {
        for (std::unique_ptr<spdlog::logger>& logger : loggers)
        {
            if (logger != nullptr)
            {
                // Write the messages that are still in the buffer to the file.
                logger->flush();

                // Destroy the logger. When the last logger is destroyed, nothing holds the sinks anymore:
                // they are destroyed too, and the log file is closed.
                logger.reset();
            }
        }
    }

    namespace Internal
    {
        bool IsLevelEnabled(LogLevel level) noexcept
        {
            // All loggers have the same minimum level, so any of them can answer.
            const spdlog::logger* logger = GetLogger(LogCategory::Core);
            return logger != nullptr && logger->should_log(ConvertToSPDLogLevel(level));
        }

        void WriteMessage(LogCategory category, LogLevel level, std::string_view message)
        {
            spdlog::logger* logger = GetLogger(category);
            if (logger != nullptr)
                logger->log(ConvertToSPDLogLevel(level), message);
        }
    }
}
