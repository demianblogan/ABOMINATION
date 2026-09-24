#include "Application/Application.h"
#include "Core/BuildConfiguration.h"
#include "Core/Log.h"
#include "Core/Version.h"
#include "Platform/SystemServices.h"

#include <expected>
#include <string>

// On laptops with hybrid graphics (an integrated and a discrete GPU) the NVIDIA / AMD driver decides which GPU runs
// a process, using its database of known games. A game that is not in the database may silently run on the weak
// integrated GPU. When the executable exports these two variables, the drivers run it on the discrete GPU instead.
// On computers with one GPU they have no effect.
// The names and types are defined by NVIDIA (Optimus) and AMD (PowerXpress) and must not be changed.
// They must be defined in the executable itself: in a static library the linker would drop them as unused.
extern "C"
{
    __declspec(dllexport) unsigned long NvOptimusEnablement = 1;
    __declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;
}

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
