#include "Win32Input.h"
#include <iostream>
#include <windowsx.h> // For GET_X_LPARAM, GET_Y_LPARAM macros

namespace System
{
Win32Input::Win32Input()
    : m_hwnd(nullptr), m_mouseX(0), m_mouseY(0), m_previousMouseX(0),
      m_previousMouseY(0), m_mouseDeltaX(0), m_mouseDeltaY(0), m_wheelDelta(0),
      m_mouseCaptured(false), m_cursorVisible(true), m_cursorShowCount(0)
{
    // Initialize all key states to false
    m_currentKeyState.fill(false);
    m_previousKeyState.fill(false);

    // Initialize all mouse button states to false
    m_currentMouseState.fill(false);
    m_previousMouseState.fill(false);

    // Initialize the key mapping
    InitializeKeyMap();
}

Win32Input::~Win32Input()
{
    Shutdown();
}

bool Win32Input::Initialize(WindowHandle windowHandle)
{
    m_hwnd = static_cast<HWND>(windowHandle);
    if (!m_hwnd)
    {
        std::cerr << "Invalid window handle provided to input system" << std::endl;
        return false;
    }

    // Get initial cursor position
    POINT cursorPos = GetCursorPosition();
    m_mouseX = m_previousMouseX = cursorPos.x;
    m_mouseY = m_previousMouseY = cursorPos.y;
    m_mouseDeltaX = m_mouseDeltaY = 0;

    return true;
}

void Win32Input::Shutdown()
{
    // Release mouse capture if we have it
    if (m_mouseCaptured)
    {
        CaptureMouse(false);
    }

    // Restore cursor visibility
    if (!m_cursorVisible)
    {
        ShowCursor(true);
    }

    // Clear callbacks
    ClearCallbacks();

    m_hwnd = nullptr;
}

void Win32Input::Update()
{
    // Copy current state to previous state
    m_previousKeyState = m_currentKeyState;
    m_previousMouseState = m_currentMouseState;

    // Update mouse position and delta
    POINT cursorPos = GetCursorPosition();
    m_previousMouseX = m_mouseX;
    m_previousMouseY = m_mouseY;
    m_mouseX = cursorPos.x;
    m_mouseY = cursorPos.y;

    m_mouseDeltaX = m_mouseX - m_previousMouseX;
    m_mouseDeltaY = m_mouseY - m_previousMouseY;

    // Reset wheel delta (it's only valid for one frame)
    m_wheelDelta = 0;
}

void Win32Input::InitializeKeyMap()
{
    // Map Windows virtual key codes to our Key enum
    // Letters (A-Z)
    for (int i = 0; i < 26; ++i)
    {
        m_keyMap['A' + i] = static_cast<Key>(static_cast<int>(Key::A) + i);
    }

    // Numbers (0-9)
    for (int i = 0; i < 10; ++i)
    {
        m_keyMap['0' + i] = static_cast<Key>(static_cast<int>(Key::Num0) + i);
    }

    // Function keys (F1-F12)
    for (int i = 0; i < 12; ++i)
    {
        m_keyMap[VK_F1 + i] = static_cast<Key>(static_cast<int>(Key::F1) + i);
    }

    // Special keys
    m_keyMap[VK_SPACE] = Key::Space;
    m_keyMap[VK_RETURN] = Key::Enter;
    m_keyMap[VK_ESCAPE] = Key::Escape;
    m_keyMap[VK_TAB] = Key::Tab;
    m_keyMap[VK_BACK] = Key::Backspace;
    m_keyMap[VK_DELETE] = Key::Delete;
    m_keyMap[VK_INSERT] = Key::Insert;
    m_keyMap[VK_HOME] = Key::Home;
    m_keyMap[VK_END] = Key::End;
    m_keyMap[VK_PRIOR] = Key::PageUp;
    m_keyMap[VK_NEXT] = Key::PageDown;

    // Arrow keys
    m_keyMap[VK_LEFT] = Key::Left;
    m_keyMap[VK_UP] = Key::Up;
    m_keyMap[VK_RIGHT] = Key::Right;
    m_keyMap[VK_DOWN] = Key::Down;

    // Modifier keys
    m_keyMap[VK_SHIFT] = Key::Shift;
    m_keyMap[VK_CONTROL] = Key::Control;
    m_keyMap[VK_MENU] = Key::Alt;
}

Key Win32Input::VirtualKeyToKey(WPARAM vk) const
{
    auto it = m_keyMap.find(vk);
    return (it != m_keyMap.end()) ? it->second : Key::Unknown;
}

MouseButton Win32Input::WParamToMouseButton(WPARAM wParam) const
{
    // This is used for wheel messages, not button messages
    // For button messages, we determine the button from the message type
    return MouseButton::Left; // Default, not used
}

void Win32Input::UpdateKeyState(Key key, bool pressed)
{
    if (key != Key::Unknown)
    {
        size_t keyIndex = static_cast<size_t>(key);
        if (keyIndex < MAX_KEYS)
        {
            m_currentKeyState[keyIndex] = pressed;
        }
    }
}

void Win32Input::UpdateMouseButtonState(MouseButton button, bool pressed)
{
    size_t buttonIndex = static_cast<size_t>(button);
    if (buttonIndex < MAX_MOUSE_BUTTONS)
    {
        m_currentMouseState[buttonIndex] = pressed;
    }
}

void Win32Input::UpdateMousePosition(int x, int y)
{
    m_mouseX = x;
    m_mouseY = y;
}

// IInput interface implementation
bool Win32Input::IsKeyDown(Key key) const
{
    size_t keyIndex = static_cast<size_t>(key);
    return (keyIndex < MAX_KEYS) ? m_currentKeyState[keyIndex] : false;
}

bool Win32Input::IsKeyUp(Key key) const
{
    return !IsKeyDown(key);
}

bool Win32Input::WasKeyPressed(Key key) const
{
    size_t keyIndex = static_cast<size_t>(key);
    if (keyIndex >= MAX_KEYS)
        return false;

    return m_currentKeyState[keyIndex] && !m_previousKeyState[keyIndex];
}

bool Win32Input::WasKeyReleased(Key key) const
{
    size_t keyIndex = static_cast<size_t>(key);
    if (keyIndex >= MAX_KEYS)
        return false;

    return !m_currentKeyState[keyIndex] && m_previousKeyState[keyIndex];
}

bool Win32Input::IsMouseButtonDown(MouseButton button) const
{
    size_t buttonIndex = static_cast<size_t>(button);
    return (buttonIndex < MAX_MOUSE_BUTTONS)
               ? m_currentMouseState[buttonIndex]
               : false;
}

bool Win32Input::IsMouseButtonUp(MouseButton button) const
{
    return !IsMouseButtonDown(button);
}

bool Win32Input::WasMouseButtonPressed(MouseButton button) const
{
    size_t buttonIndex = static_cast<size_t>(button);
    if (buttonIndex >= MAX_MOUSE_BUTTONS)
        return false;

    return m_currentMouseState[buttonIndex] &&
           !m_previousMouseState[buttonIndex];
}

bool Win32Input::WasMouseButtonReleased(MouseButton button) const
{
    size_t buttonIndex = static_cast<size_t>(button);
    if (buttonIndex >= MAX_MOUSE_BUTTONS)
        return false;

    return !m_currentMouseState[buttonIndex] &&
           m_previousMouseState[buttonIndex];
}

void Win32Input::GetMousePosition(int& x, int& y) const
{
    x = m_mouseX;
    y = m_mouseY;
}

void Win32Input::GetMouseDelta(int& deltaX, int& deltaY) const
{
    deltaX = m_mouseDeltaX;
    deltaY = m_mouseDeltaY;
}

int Win32Input::GetMouseWheelDelta() const
{
    return m_wheelDelta;
}

void Win32Input::SetKeyCallback(const KeyCallback& callback)
{
    m_keyCallback = callback;
}

void Win32Input::SetMouseButtonCallback(const MouseButtonCallback& callback)
{
    m_mouseButtonCallback = callback;
}

void Win32Input::SetMouseMoveCallback(const MouseMoveCallback& callback)
{
    m_mouseMoveCallback = callback;
}

void Win32Input::SetMouseScrollCallback(const MouseScrollCallback& callback)
{
    m_mouseScrollCallback = callback;
}

void Win32Input::ClearCallbacks()
{
    m_keyCallback = nullptr;
    m_mouseButtonCallback = nullptr;
    m_mouseMoveCallback = nullptr;
    m_mouseScrollCallback = nullptr;
}

void Win32Input::SetMousePosition(int x, int y)
{
    SetCursorPosition(x, y);
    m_mouseX = x;
    m_mouseY = y;
}

void Win32Input::ShowCursor(bool show)
{
    if (show && !m_cursorVisible)
    {
        while (::ShowCursor(TRUE) < 0)
        {
        }
        m_cursorVisible = true;
    }
    else if (!show && m_cursorVisible)
    {
        while (::ShowCursor(FALSE) >= 0)
        {
        }
        m_cursorVisible = false;
    }
}

void Win32Input::CaptureMouse(bool capture)
{
    if (capture && !m_mouseCaptured)
    {
        SetCapture(m_hwnd);
        m_mouseCaptured = true;
    }
    else if (!capture && m_mouseCaptured)
    {
        ReleaseCapture();
        m_mouseCaptured = false;
    }
}

void Win32Input::ResetInputState()
{
    m_currentKeyState.fill(false);
    m_previousKeyState.fill(false);
    m_currentMouseState.fill(false);
    m_previousMouseState.fill(false);
    m_mouseDeltaX = m_mouseDeltaY = 0;
    m_wheelDelta = 0;
}

// Message processing
void Win32Input::ProcessMessage(unsigned int message, uintptr_t wParam,
                                intptr_t lParam)
{
    switch (message)
    {
    case WM_KEYDOWN:
    case WM_SYSKEYDOWN:
        HandleKeyDown(static_cast<WPARAM>(wParam),
                      static_cast<LPARAM>(lParam));
        break;

    case WM_KEYUP:
    case WM_SYSKEYUP:
        HandleKeyUp(static_cast<WPARAM>(wParam), static_cast<LPARAM>(lParam));
        break;

    case WM_CHAR:
        HandleChar(static_cast<WPARAM>(wParam), static_cast<LPARAM>(lParam));
        break;

    case WM_LBUTTONDOWN:
    case WM_RBUTTONDOWN:
    case WM_MBUTTONDOWN:
        HandleMouseButtonDown(message, static_cast<WPARAM>(wParam),
                              static_cast<LPARAM>(lParam));
        break;

    case WM_LBUTTONUP:
    case WM_RBUTTONUP:
    case WM_MBUTTONUP:
        HandleMouseButtonUp(message, static_cast<WPARAM>(wParam),
                            static_cast<LPARAM>(lParam));
        break;

    case WM_MOUSEMOVE:
        HandleMouseMove(static_cast<WPARAM>(wParam),
                        static_cast<LPARAM>(lParam));
        break;

    case WM_MOUSEWHEEL:
        HandleMouseWheel(static_cast<WPARAM>(wParam),
                         static_cast<LPARAM>(lParam));
        break;
    }
}

void Win32Input::HandleKeyDown(WPARAM wParam, LPARAM lParam)
{
    Key key = VirtualKeyToKey(wParam);
    if (key != Key::Unknown)
    {
        bool wasPressed = !IsKeyDown(key);
        UpdateKeyState(key, true);

        // Call callback if this is a new press
        if (wasPressed)
        {
            InvokeKeyCallback(key, true);
        }
    }
}

void Win32Input::HandleKeyUp(WPARAM wParam, LPARAM lParam)
{
    Key key = VirtualKeyToKey(wParam);
    if (key != Key::Unknown)
    {
        UpdateKeyState(key, false);
        InvokeKeyCallback(key, false);
    }
}

void Win32Input::HandleSysKeyDown(WPARAM wParam, LPARAM lParam)
{
    // Handle system keys (Alt combinations, etc.)
    HandleKeyDown(wParam, lParam);
}

void Win32Input::HandleSysKeyUp(WPARAM wParam, LPARAM lParam)
{
    // Handle system keys (Alt combinations, etc.)
    HandleKeyUp(wParam, lParam);
}

void Win32Input::HandleChar(WPARAM wParam, LPARAM lParam)
{
    // Character input - could be used for text input if needed
    // For now, we handle input through key down/up events
}

void Win32Input::HandleMouseButtonDown(UINT message, WPARAM wParam, LPARAM lParam)
{
    MouseButton button = MouseButton::Left;

    switch (message)
    {
    case WM_LBUTTONDOWN:
        button = MouseButton::Left;
        break;
    case WM_RBUTTONDOWN:
        button = MouseButton::Right;
        break;
    case WM_MBUTTONDOWN:
        button = MouseButton::Middle;
        break;
    }

    bool wasPressed = !IsMouseButtonDown(button);
    UpdateMouseButtonState(button, true);

    // Get mouse position
    int x = GET_X_LPARAM(lParam);
    int y = GET_Y_LPARAM(lParam);
    UpdateMousePosition(x, y);

    // Call callback if this is a new press
    if (wasPressed)
    {
        InvokeMouseButtonCallback(button, true, x, y);
    }
}

void Win32Input::HandleMouseButtonUp(UINT message, WPARAM wParam, LPARAM lParam)
{
    MouseButton button = MouseButton::Left;

    switch (message)
    {
    case WM_LBUTTONUP:
        button = MouseButton::Left;
        break;
    case WM_RBUTTONUP:
        button = MouseButton::Right;
        break;
    case WM_MBUTTONUP:
        button = MouseButton::Middle;
        break;
    }

    UpdateMouseButtonState(button, false);

    // Get mouse position
    int x = GET_X_LPARAM(lParam);
    int y = GET_Y_LPARAM(lParam);
    UpdateMousePosition(x, y);

    InvokeMouseButtonCallback(button, false, x, y);
}

void Win32Input::HandleMouseMove(WPARAM wParam, LPARAM lParam)
{
    int x = GET_X_LPARAM(lParam);
    int y = GET_Y_LPARAM(lParam);

    UpdateMousePosition(x, y);
    InvokeMouseMoveCallback(x, y);
}

void Win32Input::HandleMouseWheel(WPARAM wParam, LPARAM lParam)
{
    // Get wheel delta (positive = forward, negative = backward)
    short delta = GET_WHEEL_DELTA_WPARAM(wParam);
    m_wheelDelta = delta / WHEEL_DELTA; // Normalize to -1, 0, or 1

    InvokeMouseScrollCallback(m_wheelDelta);
}

// Utility functions
POINT Win32Input::GetCursorPosition() const
{
    POINT point;
    GetCursorPos(&point);

    // Convert to client coordinates
    if (m_hwnd)
    {
        ScreenToClient(m_hwnd, &point);
    }

    return point;
}

void Win32Input::SetCursorPosition(int x, int y)
{
    if (m_hwnd)
    {
        POINT point = {x, y};
        ClientToScreen(m_hwnd, &point);
        SetCursorPos(point.x, point.y);
    }
}

bool Win32Input::IsKeyPressed(Key key) const
{
    return WasKeyPressed(key);
}

void Win32Input::InvokeKeyCallback(Key key, bool pressed)
{
    if (m_keyCallback)
    {
        m_keyCallback(key, pressed);
    }
}

void Win32Input::InvokeMouseButtonCallback(MouseButton button, bool pressed,
                                           int x, int y)
{
    if (m_mouseButtonCallback)
    {
        m_mouseButtonCallback(button, pressed, x, y);
    }
}

void Win32Input::InvokeMouseMoveCallback(int x, int y)
{
    if (m_mouseMoveCallback)
    {
        m_mouseMoveCallback(x, y);
    }
}

void Win32Input::InvokeMouseScrollCallback(int delta)
{
    if (m_mouseScrollCallback)
    {
        m_mouseScrollCallback(delta);
    }
}
} // namespace System