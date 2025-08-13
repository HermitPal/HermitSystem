#include "SystemFactory.h"

// Platform-specific includes
#include "Win32Input.h"
#include "Win32Window.h"

// For std::unique_ptr, std::make_shared
#include <memory>

// For WindowConfig definition
#include "IWindow.h"

namespace System
{
std::unique_ptr<IWindow>
SystemFactory::CreateApplicationWindow(const WindowConfig& config)
{
#ifdef _WIN32
    // Create the correct input system for the Windows platform.
    auto input = std::make_shared<Win32Input>();

    // Create the correct window for the Windows platform, injecting the input
    // system.
    auto window = std::make_unique<Win32Window>(input);

    // Initialize the window (which in turn initializes the input system).
    if (!window->Initialize(config))
    {
        // In a real application, you might log this error.
        return nullptr;
    }

    return window;
#else
    // If you were supporting other platforms, you would have code like this:
    // #elif __linux__
    //     auto input = std::make_shared<X11Input>();
    //     auto window = std::make_unique<X11Window>(input);
    //     if (!window->Initialize(config)) { return nullptr; }
    //     return window;

    // For now, return nullptr if not on a supported platform.
    return nullptr;
#endif
}
} // namespace System
