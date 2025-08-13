#pragma once

#include "IWindow.h"
#include <Windows.h>
#include <memory>
#include <string>

namespace Systsem
{
    /**
     * Win32Window - Windows-specific implementation of IWindow
     *
     * This class handles Win32 window creation, management, and message processing.
     * It owns and manages an input system, forwarding relevant messages to it.
     */
    class Win32Window : public IWindow
    {
      public:
        // Constructor requires input system dependency injection
        explicit Win32Window(std::shared_ptr<IInput> input);
        virtual ~Win32Window();

        // IWindow interface implementation
        bool Initialize(const WindowConfig& config) override;
        void Shutdown() override;
        void Update() override;

        // Window properties
        void SetTitle(const std::string& title) override;
        std::string GetTitle() const override;

        void SetSize(int width, int height) override;
        void GetSize(int& width, int& height) const override;

        void SetPosition(int x, int y) override;
        void GetPosition(int& x, int& y) const override;

        void SetMinimumSize(int minWidth, int minHeight) override;
        void SetMaximumSize(int maxWidth, int maxHeight) override;

        // Window state
        void Show() override;
        void Hide() override;
        void Minimize() override;
        void Maximize() override;
        void Restore() override;

        bool IsVisible() const override;
        bool IsMinimized() const override;
        bool IsMaximized() const override;
        bool HasFocus() const override;
        bool ShouldClose() const override;

        // Fullscreen support
        void SetFullscreen(bool fullscreen) override;
        bool IsFullscreen() const override;

        // VSync control (requires graphics context - placeholder implementation)
        void SetVSync(bool enabled) override;
        bool IsVSyncEnabled() const override;

        // Platform-specific handle access
        WindowHandle GetNativeHandle() const override;

        // Input system access
        std::shared_ptr<IInput> GetInput() const override;

        // Event callbacks
        void SetResizeCallback(const WindowResizeCallback& callback) override;
        void SetCloseCallback(const WindowCloseCallback& callback) override;
        void SetFocusCallback(const WindowFocusCallback& callback) override;
        void ClearCallbacks() override;

        // Utility functions
        void RequestClose() override;
        void SetIcon(const std::string& iconPath) override;

        // Client area functions
        void GetClientSize(int& width, int& height) const override;
        void ClientToScreen(int& x, int& y) const override;
        void ScreenToClient(int& x, int& y) const override;

      private:
        // Win32-specific data
        HWND m_hwnd;
        HINSTANCE m_hinstance;
        std::wstring m_className;
        WindowConfig m_config;

        // Window state tracking
        bool m_isFullscreen;
        bool m_vsyncEnabled;
        RECT m_windowedRect; // For restoring from fullscreen
        DWORD m_windowedStyle;
        DWORD m_windowedExStyle;

        // Size constraints
        int m_minWidth, m_minHeight;
        int m_maxWidth, m_maxHeight;

        // Event callbacks
        WindowResizeCallback m_resizeCallback;
        WindowCloseCallback m_closeCallback;
        WindowFocusCallback m_focusCallback;

        // Private methods
        bool RegisterWindowClass();
        void UnregisterWindowClass();
        HWND CreateWindowHandle(const WindowConfig& config);
        void UpdateWindowStyle();

        // Window procedure
        static LRESULT CALLBACK StaticWindowProc(HWND hwnd, UINT message, WPARAM wParam,
                                                 LPARAM lParam);
        LRESULT WindowProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

        // Message handlers
        void HandleResize(int width, int height);
        void HandleClose();
        void HandleFocus(bool hasFocus);
        void HandleGetMinMaxInfo(MINMAXINFO* minMaxInfo);

        // Utility functions
        DWORD GetWindowStyle(const WindowConfig& config) const;
        DWORD GetWindowExStyle(const WindowConfig& config) const;
        void AdjustWindowSizeForStyle(int& width, int& height, DWORD style,
                                      DWORD exStyle) const;
        std::wstring StringToWString(const std::string& str) const;
        std::string WStringToString(const std::wstring& wstr) const;

        // Static members for window class management
        static bool s_classRegistered;
        static int s_windowCount;
        static const wchar_t* s_className;
    };
} // namespace Systsem