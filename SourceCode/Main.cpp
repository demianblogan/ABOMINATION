#include "Core/BuildConfiguration.h"
#include "Core/Log.h"
#include "Core/Version.h"

int main()
{
    using namespace Abomination::Core;

    Log::Initialize(LogSettings{
        .filePath = "Abomination.log",
        .minimumLevel = IsDebugBuild ? LogLevel::Debug : LogLevel::Info,
    });

    Log::Write(LogCategory::Core, LogLevel::Info, "Abomination {}", GetGameVersionString());
    Log::Write(LogCategory::Core, LogLevel::Debug, "Debug build: detailed logging is enabled");

    Log::Shutdown();

    return 0;
}
