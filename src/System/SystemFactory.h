#pragma once

#include "IWindow.h"
#include <memory>

namespace Systsem
{
    // Forward declare WindowConfig, as IWindow.h includes it, but it's good practice
    // for headers to be self-contained where possible.
    struct WindowConfig;

    /**
     * SystemFactory - A factory for creating platform-specific application components.
     *
     * This class abstracts the creation of platform-dependent objects like windows
     * and input systems, ensuring that compatible components are always created
     * together.
     */
    class SystemFactory
    {
      public:
        /**
         * @brief Creates a platform-specific window and its corresponding input
         * system.
         *
         * This static method is the entry point for creating the main application
         * window. It handles the instantiation of the correct concrete window and
         * input classes for the target platform. The returned window object owns the
         * input system.
         *
         * @param config The configuration for the window to be created.
         * @return A unique pointer to an IWindow interface. Returns nullptr on
         * failure.
         */
        static std::unique_ptr<IWindow>
        CreateApplicationWindow(const WindowConfig& config);
    };
} // namespace Systsem
