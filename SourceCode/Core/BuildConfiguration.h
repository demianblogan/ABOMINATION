#pragma once

namespace Abomination::Core
{
    // True in Debug builds, false in Release. Use in `if constexpr` instead of #ifdef.
#ifdef NDEBUG
    inline constexpr bool IsDebugBuild = false;
#else
    inline constexpr bool IsDebugBuild = true;
#endif
}
