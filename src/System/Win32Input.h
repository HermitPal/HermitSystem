#pragma once

#include "IInput.h"
#include <Windows.h>
#include <array>
#include <unordered_map>

namespace Systsem
{
    /**
     * Win32Input - Windows-specific implementation of IInput
     *
     * This class handles Win32 input processing using Windows messages.
     * It maintains both current and previous frame state to support
     * both polling and event-driven input handling.
     */
    class Win32Input : public IInput
    {
      public:
        Win32Input();
        virtual ~Win32Input();

        // IInput interface implementation
        bool Initialize(WindowHandle windowHandle) override;
        void Shutdown() override;
        void Update() override;

        // Keyboard input - polling interface
        bool IsKeyDown(Key key) const override;
        bool IsKeyUp(Key key) const override;
        bool WasKeyPressed(Key key) const override;
        bool WasKeyReleased(Key key) const override;

        // Mouse input - polling interface
        bool IsMouseButtonDown(MouseButton button) const override;
        bool IsMouseButtonUp(MouseButton button) const override;
        bool WasMouseButtonPressed(MouseButton button) const override;
        bool WasMouseButtonReleased(MouseButton button) const override;

        // Mouse position and movement
        void GetMousePosition(int& x, int& y) const override;
        void GetMouseDelta(int& deltaX, int& deltaY) const override;
        int GetMouseWheelDelta() const override;

        // Event-driven interface - callbacks
        void SetKeyCallback(const KeyCallback& callback) override;
        void SetMouseButtonCallback(const MouseButtonCallback& callback) override;
        void SetMouseMoveCallback(const MouseMoveCallback& callback) override;
        void SetMouseScrollCallback(const MouseScrollCallback& callback) override;

        // Clear callbacks
        void ClearCallbacks() override;

        // Utility functions
        void SetMousePosition(int x, int y) override;
        void ShowCursor(bool show) override;
        void CaptureMouse(bool capture) override;

        // Input state management
        void ResetInputState() override;

        // Platform-specific message processing - not part of the interface
        void ProcessMessage(unsigned int message, uintptr_t wParam, intptr_t lParam);

      private:
        // Constants
        static constexpr size_t MAX_KEYS = 512;
        static constexpr size_t MAX_MOUSE_BUTTONS = 3;

        // Window handle
        HWND m_hwnd;

        // Keyboard state
        std::array<bool, MAX_KEYS> m_currentKeyState;
        std::array<bool, MAX_KEYS> m_previousKeyState;

        // Mouse state
        std::array<bool, MAX_MOUSE_BUTTONS> m_currentMouseState;
        std::array<bool, MAX_MOUSE_BUTTONS> m_previousMouseState;

        // Mouse position and movement
        int m_mouseX, m_mouseY;
        int m_previousMouseX, m_previousMouseY;
        int m_mouseDeltaX, m_mouseDeltaY;
        int m_wheelDelta;

        // Mouse capture and cursor state
        bool m_mouseCaptured;
        bool m_cursorVisible;
        int m_cursorShowCount; // Track cursor show/hide count

        // Event callbacks
        KeyCallback m_keyCallback;
        MouseButtonCallback m_mouseButtonCallback;
        MouseMoveCallback m_mouseMoveCallback;
        MouseScrollCallback m_mouseScrollCallback;

        // Key mapping
        std::unordered_map<WPARAM, Key> m_keyMap;

        // Private methods
        void InitializeKeyMap();
        Key VirtualKeyToKey(WPARAM vk) const;
        MouseButton WParamToMouseButton(WPARAM wParam) const;
        void UpdateKeyState(Key key, bool pressed);
        void UpdateMouseButtonState(MouseButton button, bool pressed);
        void UpdateMousePosition(int x, int y);

        // Message handlers
        void HandleKeyDown(WPARAM wParam, LPARAM lParam);
        void HandleKeyUp(WPARAM wParam, LPARAM lParam);
        void HandleSysKeyDown(WPARAM wParam, LPARAM lParam);
        void HandleSysKeyUp(WPARAM wParam, LPARAM lParam);
        void HandleChar(WPARAM wParam, LPARAM lParam);

        void HandleMouseButtonDown(UINT message, WPARAM wParam, LPARAM lParam);
        void HandleMouseButtonUp(UINT message, WPARAM wParam, LPARAM lParam);
        void HandleMouseMove(WPARAM wParam, LPARAM lParam);
        void HandleMouseWheel(WPARAM wParam, LPARAM lParam);

        // Utility functions
        POINT GetCursorPosition() const;
        void SetCursorPosition(int x, int y);
        bool IsKeyPressed(Key key) const;
        void InvokeKeyCallback(Key key, bool pressed);
        void InvokeMouseButtonCallback(MouseButton button, bool pressed, int x, int y);
        void InvokeMouseMoveCallback(int x, int y);
        void InvokeMouseScrollCallback(int delta);
    };
} // namespace Systsem