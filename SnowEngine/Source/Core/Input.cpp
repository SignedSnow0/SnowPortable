#include "Input.h"
#include "Logger.h"

namespace Snow {
    b8 Input::sKeys[512]{ false };
    b8 Input::sMouseButtons[8]{ false };
    vec2f Input::sMousePosition{ 0, 0 };
    f32 Input::sWheelDelta{ 0 };

    b8 Input::KeyPressed(Key key) { return sKeys[static_cast<u32>(key)]; }

    b8 Input::ButtonPressed(Button button) { return sMouseButtons[static_cast<u32>(button)]; }
    
    vec2f Input::MousePosition() { return sMousePosition; }
    
    f32 Input::MouseX() { return sMousePosition.x; }
    
    f32 Input::MouseY() { return sMousePosition.y; }

    f32 Input::WheelDelta() { return sWheelDelta; }

    void Input::SetKey(Key key, b8 pressed) {
        u32 i{ static_cast<u32>(key) };
        if (i >= 512) {
            LOG_WARN("Unknown key pressed: {}", i);
            return;
        }
        sKeys[i] = pressed;
    }
    
    void Input::SetButton(Button button, b8 pressed) {
        u32 i{ static_cast<u32>(button) };
        if (i >= 8) {
            LOG_WARN("Unknown button pressed: {}", i);
            return;
        }
        sMouseButtons[static_cast<u32>(button)] = pressed;
    }
    
    void Input::SetMousePosition(vec2f position) { sMousePosition = position; }
    
    void Input::SetWheelDelta(f32 delta) { sWheelDelta = delta; }
}