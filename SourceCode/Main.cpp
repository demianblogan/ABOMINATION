#include "Application/Application.h"
#include "Core/BuildConfiguration.h"
#include "Core/Log.h"
#include "Core/Version.h"
#include "Platform/SystemServices.h"

#include <expected>
#include <string>

namespace
{
    using namespace Abomination;
    using namespace Abomination::Core;

    // Creates and runs the application. It is a separate function so that the application (the window, SDL)
    // is destroyed when the function returns, while logging still works and can record the shutdown.
    int RunApplication()
    {
        std::expected<Application, std::string> application = Application::Create();
        if (!application.has_value())
        {
            Log::Write(LogCategory::Core, LogLevel::Critical, "{}", application.error());
            Platform::ShowErrorDialog("Abomination", application.error());

            return 1;
        }

        return application->Run();
    }
}

int main()
{
    Log::Initialize(LogSettings{
        .filePath = Platform::GetExecutableDirectory() / "Abomination.log",
        .minimumLevel = IsDebugBuild ? LogLevel::Debug : LogLevel::Info,
    });

    Log::Write(LogCategory::Core, LogLevel::Info, "Abomination {}", GetGameVersionString());

    const int exitCode = RunApplication();

    Log::Shutdown();

    return exitCode;
}
