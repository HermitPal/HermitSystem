#include "Win32Window.h"
#include "IInput.h"
#include "Win32Input.h" // For dynamic_pointer_cast
#include <iostream>
#include <stdexcept>

namespace System
{
// Static member definitions
bool Win32Window::s_classRegistered = false;
int Win32Window::s_windowCount = 0;
const wchar_t* Win32Window::s_className = L"Win32WindowClass";

Win32Window::Win32Window(std::shared_ptr<IInput> input)
    : m_hwnd(nullptr), m_hinstance(GetModuleHandle(nullptr)),
      m_isFullscreen(false), m_vsyncEnabled(true), m_minWidth(320),
      m_minHeight(240), m_maxWidth(-1), m_maxHeight(-1)
{
    // Store the input system - dependency injection enforced at construction
    if (!input)
    {
        throw std::invalid_argument("Input system cannot be null");
    }
    m_input = input;

    ++s_windowCount;
}

Win32Window::~Win32Window()
{
    if (m_isInitialized)
    {
        Shutdown();
    }

    --s_windowCount;
    if (s_windowCount == 0 && s_classRegistered)
    {
        UnregisterWindowClass();
    }
}

bool Win32Window::Initialize(const WindowConfig& config)
{
    if (m_isInitialized)
    {
        std::cerr << "Window already initialized" << std::endl;
        return false;
    }

    std::cout << "Initializing window..." << std::endl;
    std::cout << "Title: " << config.title << std::endl;
    std::cout << "Size: " << config.width << "x" << config.height << std::endl;

    m_config = config;

    // Register window class if not already registered
    std::cout << "Registering window class..." << std::endl;
    if (!RegisterWindowClass())
    {
        std::cerr << "Failed to register window class" << std::endl;
        return false;
    }
    std::cout << "Window class registered successfully" << std::endl;

    // Create the window
    std::cout << "Creating window handle..." << std::endl;
    m_hwnd = CreateWindowHandle(config);
    if (!m_hwnd)
    {
        std::cerr << "Failed to create window handle" << std::endl;
        return false;
    }
    std::cout << "Window handle created successfully: " << m_hwnd << std::endl;

    // Initialize input system with the window handle
    // This is the key part - window initializes input after it has a valid
    // handle
    std::cout << "Initializing input system..." << std::endl;
    if (!m_input->Initialize(m_hwnd))
    {
        std::cerr << "Failed to initialize input system" << std::endl;
        DestroyWindow(m_hwnd);
        m_hwnd = nullptr;
        return false;
    }
    std::cout << "Input system initialized successfully" << std::endl;

    // Set initial window state
    if (config.maximized)
    {
        ShowWindow(m_hwnd, SW_SHOWMAXIMIZED);
    }
    else if (config.fullscreen)
    {
        SetFullscreen(true);
        ShowWindow(m_hwnd, SW_SHOW);
    }
    else
    {
        ShowWindow(m_hwnd, SW_SHOW);
    }

    UpdateWindow(m_hwnd);
    std::cout << "Window initialization complete" << std::endl;

    m_isInitialized = true;
    return true;
}

void Win32Window::Shutdown()
{
    if (!m_isInitialized)
    {
        return;
    }

    // Shutdown input system first
    if (m_input)
    {
        m_input->Shutdown();
    }

    // Destroy window
    if (m_hwnd)
    {
        DestroyWindow(m_hwnd);
        m_hwnd = nullptr;
    }

    m_isInitialized = false;
}

void Win32Window::Update()
{
    if (!m_isInitialized)
    {
        return;
    }

    // Process Windows messages
    MSG msg;
    while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    // Update input system
    if (m_input)
    {
        m_input->Update();
    }
}

bool Win32Window::RegisterWindowClass()
{
    if (s_classRegistered)
    {
        return true;
    }

    WNDCLASSEXW wcex = {};
    wcex.cbSize = sizeof(WNDCLASSEXW);

    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = StaticWindowProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = m_hinstance;
    wcex.hIcon = LoadIcon(nullptr, IDI_APPLICATION);
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = nullptr;
    wcex.lpszClassName = s_className;
    wcex.hIconSm = LoadIcon(nullptr, IDI_APPLICATION);

    ATOM result = RegisterClassExW(&wcex);
    if (result == 0)
    {
        DWORD error = GetLastError();
        std::cerr << "Failed to register window class. Error: " << error
                  << std::endl;

        // Common error codes
        switch (error)
        {
        case ERROR_INVALID_PARAMETER:
            std::cerr << "ERROR_INVALID_PARAMETER (1400) - One of the "
                         "parameters is invalid"
                      << std::endl;
            std::cerr << "Check: window class, style flags, size values, "
                         "instance handle"
                      << std::endl;
            break;
        case ERROR_CLASS_DOES_NOT_EXIST:
            std::cerr
                << "ERROR_CLASS_DOES_NOT_EXIST - Window class not found"
                << std::endl;
            break;
        // case ERROR_INVALID_WINDOW_HANDLE: // This is 1400 too, but
        // different
        // context
        //   std::cerr << "ERROR_INVALID_WINDOW_HANDLE - Invalid parent window
        //   handle"
        //             << std::endl;
        //   break;
        default:
            std::cerr << "Unknown error registering window class" << std::endl;
            break;
        }
        return false;
    }

    s_classRegistered = true;
    return true;
}

void Win32Window::UnregisterWindowClass()
{
    if (s_classRegistered)
    {
        UnregisterClassW(s_className, m_hinstance);
        s_classRegistered = false;
    }
}

// Add this debug output to CreateWindowHandle function
HWND Win32Window::CreateWindowHandle(const WindowConfig& config)
{
    DWORD style = GetWindowStyle(config);
    DWORD exStyle = GetWindowExStyle(config);

    // Calculate window size including borders/title bar
    int windowWidth = config.width;
    int windowHeight = config.height;
    AdjustWindowSizeForStyle(windowWidth, windowHeight, style, exStyle);

    // Calculate position
    int x = config.posX;
    int y = config.posY;
    if (x < 0 || y < 0)
    {
        x = CW_USEDEFAULT;
        y = CW_USEDEFAULT;
    }

    std::wstring title = StringToWString(config.title);

    // Debug output to verify parameters
    std::wcout << L"Creating window with class: " << s_className << std::endl;
    std::wcout << L"Title: " << title << std::endl;
    std::wcout << L"Style: " << std::hex << style << std::endl;
    std::wcout << L"ExStyle: " << std::hex << exStyle << std::endl;
    std::wcout << L"Position: " << std::dec << x << ", " << y << std::endl;
    std::wcout << L"Size: " << windowWidth << "x" << windowHeight << std::endl;
    std::wcout << L"Instance: " << m_hinstance << std::endl;

    std::wcout << L"Verifying instance handle..." << std::endl;
    if (!m_hinstance)
    {
        std::wcout << L"Instance handle is NULL!" << std::endl;
        return nullptr;
    }

    HWND hwnd = CreateWindowExW(exStyle,       // Extended window style
                                s_className,   // Window class name
                                title.c_str(), // Window title
                                style,         // Window style
                                x,             // X position
                                y,             // Y position
                                windowWidth,   // Width
                                windowHeight,  // Height
                                nullptr,       // Parent window
                                nullptr,       // Menu
                                m_hinstance,   // Instance handle
                                this           // Additional application data
    );

    if (!hwnd)
    {
        DWORD error = GetLastError();
        std::cerr << "CreateWindowExW failed with error: " << error
                  << std::endl;

        // More detailed error reporting
        switch (error)
        {
        case ERROR_INVALID_PARAMETER:
            std::cerr << "ERROR_INVALID_PARAMETER (1400) - One of the "
                         "parameters is invalid"
                      << std::endl;
            std::cerr
                << "Check: window class registration, style flags, size values"
                << std::endl;
            break;
        case ERROR_CLASS_DOES_NOT_EXIST:
            std::cerr
                << "ERROR_CLASS_DOES_NOT_EXIST - Window class not found"
                << std::endl;
            break;
        case ERROR_INVALID_WINDOW_HANDLE:
            std::cerr << "ERROR_INVALID_WINDOW_HANDLE - Invalid parent "
                         "window handle"
                      << std::endl;
            break;
        case ERROR_NOT_ENOUGH_MEMORY:
            std::cerr << "ERROR_NOT_ENOUGH_MEMORY - Insufficient memory"
                      << std::endl;
            break;
        default:
            std::cerr << "Unknown error: " << error << std::endl;
            break;
        }

        // Verify the window class is actually registered
        WNDCLASSEXW wcex = {};
        wcex.cbSize = sizeof(WNDCLASSEXW);
        if (GetClassInfoExW(m_hinstance, s_className, &wcex))
        {
            std::wcout << L"Window class IS registered" << std::endl;
        }
        else
        {
            std::wcout << L"Window class is NOT registered!" << std::endl;
            DWORD classError = GetLastError();
            std::cerr << "GetClassInfoExW error: " << classError << std::endl;
        }
    }

    return hwnd;
}

LRESULT CALLBACK Win32Window::StaticWindowProc(HWND hwnd, UINT message,
                                               WPARAM wParam, LPARAM lParam)
{
    Win32Window* window = nullptr;

    if (message == WM_NCCREATE)
    {
        // Extract the 'this' pointer from the creation parameters
        CREATESTRUCT* pCreate = reinterpret_cast<CREATESTRUCT*>(lParam);
        window = reinterpret_cast<Win32Window*>(pCreate->lpCreateParams);
        SetWindowLongPtr(hwnd, GWLP_USERDATA,
                         reinterpret_cast<LONG_PTR>(window));

        // CRITICAL: Must return TRUE for WM_NCCREATE
        if (window)
        {
            return window->WindowProc(hwnd, message, wParam, lParam);
        }
        return TRUE; // Allow window creation to continue
    }
    else
    {
        // Retrieve the 'this' pointer
        window = reinterpret_cast<Win32Window*>(
            GetWindowLongPtr(hwnd, GWLP_USERDATA));
    }

    if (window)
    {
        return window->WindowProc(hwnd, message, wParam, lParam);
    }

    return DefWindowProc(hwnd, message, wParam, lParam);
}

LRESULT Win32Window::WindowProc(HWND hwnd, UINT message, WPARAM wParam,
                                LPARAM lParam)
{
    // Handle critical creation messages first
    switch (message)
    {
    case WM_NCCREATE:
        return TRUE; // Must return TRUE to continue window creation

    case WM_CREATE:
        return 0; // Return 0 to continue window creation
    }

    // Forward input messages to the input system by casting to the concrete
    // Win32 implementation. This avoids polluting the IInput interface with
    // platform-specific methods.
    if (auto win32Input = std::dynamic_pointer_cast<Win32Input>(m_input))
    {
        switch (message)
        {
        case WM_KEYDOWN:
        case WM_KEYUP:
        case WM_SYSKEYDOWN:
        case WM_SYSKEYUP:
        case WM_CHAR:
        case WM_LBUTTONDOWN:
        case WM_LBUTTONUP:
        case WM_RBUTTONDOWN:
        case WM_RBUTTONUP:
        case WM_MBUTTONDOWN:
        case WM_MBUTTONUP:
        case WM_MOUSEMOVE:
        case WM_MOUSEWHEEL:
            win32Input->ProcessMessage(message, wParam, lParam);
            break;
        }
    }

    // Handle window-specific messages
    switch (message)
    {
    case WM_SIZE: {
        int width = LOWORD(lParam);
        int height = HIWORD(lParam);
        HandleResize(width, height);
        return 0;
    }

    case WM_CLOSE:
        HandleClose();
        return 0;

    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;

    case WM_SETFOCUS:
        HandleFocus(true);
        return 0;

    case WM_KILLFOCUS:
        HandleFocus(false);
        return 0;

    case WM_GETMINMAXINFO: {
        MINMAXINFO* minMaxInfo = reinterpret_cast<MINMAXINFO*>(lParam);
        HandleGetMinMaxInfo(minMaxInfo);
        return 0;
    }

    case WM_ERASEBKGND:
        return 1; // Prevent background erasing

    default:
        return DefWindowProc(hwnd, message, wParam, lParam);
    }
}

void Win32Window::HandleResize(int width, int height)
{
    if (m_resizeCallback)
    {
        m_resizeCallback(width, height);
    }
}

void Win32Window::HandleClose()
{
    if (m_closeCallback)
    {
        m_closeCallback();
    }
    m_shouldClose = true;
}

void Win32Window::HandleFocus(bool hasFocus)
{
    if (m_focusCallback)
    {
        m_focusCallback(hasFocus);
    }
}

void Win32Window::HandleGetMinMaxInfo(MINMAXINFO* minMaxInfo)
{
    if (m_minWidth > 0 && m_minHeight > 0)
    {
        minMaxInfo->ptMinTrackSize.x = m_minWidth;
        minMaxInfo->ptMinTrackSize.y = m_minHeight;
    }

    if (m_maxWidth > 0 && m_maxHeight > 0)
    {
        minMaxInfo->ptMaxTrackSize.x = m_maxWidth;
        minMaxInfo->ptMaxTrackSize.y = m_maxHeight;
    }
}

// IWindow interface implementations
void Win32Window::SetTitle(const std::string& title)
{
    if (m_hwnd)
    {
        std::wstring wideTitle = StringToWString(title);
        SetWindowTextW(m_hwnd, wideTitle.c_str());
    }
    m_config.title = title;
}

std::string Win32Window::GetTitle() const
{
    return m_config.title;
}

void Win32Window::SetSize(int width, int height)
{
    if (m_hwnd && !m_isFullscreen)
    {
        DWORD style = GetWindowLong(m_hwnd, GWL_STYLE);
        DWORD exStyle = GetWindowLong(m_hwnd, GWL_EXSTYLE);

        int windowWidth = width;
        int windowHeight = height;
        AdjustWindowSizeForStyle(windowWidth, windowHeight, style, exStyle);

        SetWindowPos(m_hwnd, nullptr, 0, 0, windowWidth, windowHeight,
                     SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE);
    }
}

void Win32Window::GetSize(int& width, int& height) const
{
    if (m_hwnd)
    {
        RECT rect;
        GetWindowRect(m_hwnd, &rect);
        width = rect.right - rect.left;
        height = rect.bottom - rect.top;
    }
    else
    {
        width = m_config.width;
        height = m_config.height;
    }
}

void Win32Window::GetClientSize(int& width, int& height) const
{
    if (m_hwnd)
    {
        RECT rect;
        GetClientRect(m_hwnd, &rect);
        width = rect.right - rect.left;
        height = rect.bottom - rect.top;
    }
    else
    {
        width = m_config.width;
        height = m_config.height;
    }
}

void Win32Window::Show()
{
    if (m_hwnd)
    {
        ShowWindow(m_hwnd, SW_SHOW);
    }
}

void Win32Window::Hide()
{
    if (m_hwnd)
    {
        ShowWindow(m_hwnd, SW_HIDE);
    }
}

bool Win32Window::IsVisible() const
{
    return m_hwnd ? IsWindowVisible(m_hwnd) != FALSE : false;
}

bool Win32Window::ShouldClose() const
{
    return m_shouldClose;
}

WindowHandle Win32Window::GetNativeHandle() const
{
    return m_hwnd;
}

std::shared_ptr<IInput> Win32Window::GetInput() const
{
    return m_input;
}

void Win32Window::SetResizeCallback(const WindowResizeCallback& callback)
{
    m_resizeCallback = callback;
}

void Win32Window::SetCloseCallback(const WindowCloseCallback& callback)
{
    m_closeCallback = callback;
}

void Win32Window::SetFocusCallback(const WindowFocusCallback& callback)
{
    m_focusCallback = callback;
}

void Win32Window::RequestClose()
{
    m_shouldClose = true;
}

// Utility functions
DWORD Win32Window::GetWindowStyle(const WindowConfig& config) const
{
    if (config.fullscreen)
    {
        return WS_POPUP;
    }

    DWORD style = WS_OVERLAPPEDWINDOW;
    if (!config.resizable)
    {
        style &= ~(WS_THICKFRAME | WS_MAXIMIZEBOX);
    }
    return style;
}

DWORD Win32Window::GetWindowExStyle(const WindowConfig& config) const
{
    return WS_EX_APPWINDOW;
}

void Win32Window::AdjustWindowSizeForStyle(int& width, int& height, DWORD style,
                                           DWORD exStyle) const
{
    RECT rect = {0, 0, width, height};
    AdjustWindowRectEx(&rect, style, FALSE, exStyle);
    width = rect.right - rect.left;
    height = rect.bottom - rect.top;
}

std::wstring Win32Window::StringToWString(const std::string& str) const
{
    if (str.empty())
        return std::wstring();

    int size = MultiByteToWideChar(CP_UTF8, 0, str.c_str(),
                                   static_cast<int>(str.length()), nullptr, 0);
    if (size == 0)
    {
        // Fallback: try with current code page
        size = MultiByteToWideChar(CP_ACP, 0, str.c_str(),
                                   static_cast<int>(str.length()), nullptr, 0);
        if (size == 0)
        {
            std::cerr << "Failed to convert string to wide string"
                      << std::endl;
            return std::wstring();
        }
    }

    std::wstring wstr(size, 0);
    MultiByteToWideChar(CP_UTF8, 0, str.c_str(),
                        static_cast<int>(str.length()), &wstr[0], size);
    return wstr;
}

// Placeholder implementations for unsupported features
void Win32Window::SetPosition(int x, int y)
{ /* Implementation needed */
}
void Win32Window::GetPosition(int& x, int& y) const
{ /* Implementation needed */
}
void Win32Window::SetMinimumSize(int minWidth, int minHeight)
{
    m_minWidth = minWidth;
    m_minHeight = minHeight;
}
void Win32Window::SetMaximumSize(int maxWidth, int maxHeight)
{
    m_maxWidth = maxWidth;
    m_maxHeight = maxHeight;
}
void Win32Window::Minimize()
{
    if (m_hwnd)
        ShowWindow(m_hwnd, SW_MINIMIZE);
}
void Win32Window::Maximize()
{
    if (m_hwnd)
        ShowWindow(m_hwnd, SW_MAXIMIZE);
}
void Win32Window::Restore()
{
    if (m_hwnd)
        ShowWindow(m_hwnd, SW_RESTORE);
}
bool Win32Window::IsMinimized() const
{
    return false;
}
bool Win32Window::IsMaximized() const
{
    return false;
}
bool Win32Window::HasFocus() const
{
    return m_hwnd == GetForegroundWindow();
}
void Win32Window::SetFullscreen(bool fullscreen)
{
    m_isFullscreen = fullscreen;
}
bool Win32Window::IsFullscreen() const
{
    return m_isFullscreen;
}
void Win32Window::SetVSync(bool enabled)
{
    m_vsyncEnabled = enabled;
}
bool Win32Window::IsVSyncEnabled() const
{
    return m_vsyncEnabled;
}
void Win32Window::ClearCallbacks()
{
    m_resizeCallback = nullptr;
    m_closeCallback = nullptr;
    m_focusCallback = nullptr;
}
void Win32Window::SetIcon(const std::string& iconPath)
{ /* Implementation needed */
}
void Win32Window::ClientToScreen(int& x, int& y) const
{ /* Implementation needed */
}
void Win32Window::ScreenToClient(int& x, int& y) const
{ /* Implementation needed */
}
} // namespace System