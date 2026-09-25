#include "Platform/SystemServices.h"

#include <SDL3/SDL.h>

namespace Abomination::Platform
{
    std::filesystem::path GetExecutableDirectory()
    {
        // SDL owns the returned string, it must not be freed. nullptr means the folder could not be determined.
        const char* basePath = SDL_GetBasePath();
        if (basePath == nullptr)
            return std::filesystem::current_path();

        // The path is UTF-8; char8_t tells std::filesystem to decode it as UTF-8, not in the system code page.
        return std::filesystem::path(reinterpret_cast<const char8_t*>(basePath));
    }

    void SleepPrecisely(std::chrono::nanoseconds duration)
    {
        if (duration <= std::chrono::nanoseconds::zero())
            return;

        SDL_DelayPrecise(static_cast<Uint64>(duration.count()));
    }

    void ShowErrorDialog(const std::string& title, const std::string& message)
    {
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, title.c_str(), message.c_str(), nullptr);
    }
}
