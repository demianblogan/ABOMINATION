#pragma once

#include "Input/Keyboard.h"
#include "Input/Mouse.h"

namespace Abomination::Input
{
    // The state of all input devices for the current frame. The platform layer fills it once per frame,
    // the game reads it. A gamepad is added here in milestone 0.7.
    struct InputDevices
    {
        Keyboard keyboard;
        Mouse mouse;
    };
}
