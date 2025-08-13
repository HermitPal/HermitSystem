#pragma once

#include <functional>
#include <memory>

namespace System
{
// Forward declarations
using WindowHandle = void*;

// Key codes (can be extended based on platform needs)
// clang-format off
enum class Key {
    Unknown = 0,
    
    // Letters
    A = 65, B, C, D, E, F, G, H, I, J, K, L, M, N, O, P, Q, R, S, T, U, V, W, X, Y, Z,
    
    // Numbers
    Num0 = 48, Num1, Num2, Num3, Num4, Num5, Num6, Num7, Num8, Num9,
    
    // Function keys
    F1 = 112, F2, F3, F4, F5, F6, F7, F8, F9, F10, F11, F12,
    
    // Special keys
    Space = 32,
    Enter = 13,
    Escape = 27,
    Tab = 9,
    Backspace = 8,
    Delete = 46,
    Insert = 45,
    Home = 36,
    End = 35,
    PageUp = 33,
    PageDown = 34,
    
    // Arrow keys
    Left = 37,
    Up = 38,
    Right = 39,
    Down = 40,
    
    // Modifier keys
    Shift = 16,
    Control = 17,
    Alt = 18,
    
    // Mouse buttons (handled as keys for simplicity)
    MouseLeft = 256,
    MouseRight = 257,
    MouseMiddle = 258
};
// clang-format on

// Mouse button enumeration
enum class MouseButton
{
    Left = 0,
    Right = 1,
    Middle = 2
};

// Input event callbacks
using KeyCallback = std::function<void(Key key, bool pressed)>;
using MouseButtonCallback =
    std::function<void(MouseButton button, bool pressed, int x, int y)>;
using MouseMoveCallback = std::function<void(int x, int y)>;
using MouseScrollCallback = std::function<void(int delta)>;

/**
 * IInput - Abstract interface for input handling
 *
 * This interface provides platform-independent input functionality.
 * It supports both polling (IsKeyDown) and event-driven (callbacks) input
 * handling.
 */
class IInput
{
  public:
    virtual ~IInput() = default;

    // Initialization - called by the window after it has a valid handle
    virtual bool Initialize(WindowHandle windowHandle) = 0;
    virtual void Shutdown() = 0;

    // Frame update - called each frame to process accumulated input
    virtual void Update() = 0;

    // Keyboard input - polling interface
    virtual bool IsKeyDown(Key key) const = 0;
    virtual bool IsKeyUp(Key key) const = 0;
    virtual bool
    WasKeyPressed(Key key) const = 0; // True for one frame when key is first pressed
    virtual bool
    WasKeyReleased(Key key) const = 0; // True for one frame when key is released

    // Mouse input - polling interface
    virtual bool IsMouseButtonDown(MouseButton button) const = 0;
    virtual bool IsMouseButtonUp(MouseButton button) const = 0;
    virtual bool WasMouseButtonPressed(MouseButton button) const = 0;
    virtual bool WasMouseButtonReleased(MouseButton button) const = 0;

    // Mouse position and movement
    virtual void GetMousePosition(int& x, int& y) const = 0;
    virtual void GetMouseDelta(int& deltaX, int& deltaY) const = 0;
    virtual int GetMouseWheelDelta() const = 0;

    // Event-driven interface - callbacks
    virtual void SetKeyCallback(const KeyCallback& callback) = 0;
    virtual void SetMouseButtonCallback(const MouseButtonCallback& callback) = 0;
    virtual void SetMouseMoveCallback(const MouseMoveCallback& callback) = 0;
    virtual void SetMouseScrollCallback(const MouseScrollCallback& callback) = 0;

    // Clear callbacks
    virtual void ClearCallbacks() = 0;

    // Utility functions
    virtual void SetMousePosition(int x, int y) = 0;
    virtual void ShowCursor(bool show) = 0;
    virtual void CaptureMouse(bool capture) = 0;

    // Input state management
    virtual void ResetInputState() = 0;
};
} // namespace System