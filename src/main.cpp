#include "SystemFactory.h"
#include "IInput.h"
#include "IWindow.h"
#include <iostream>
#include <memory>

// For Sleep()
#ifdef _WIN32
#include <Windows.h>
#endif

// Use the system namespace to avoid qualifying every type
using namespace Systsem;

int main()
{
    try
    {
        // Step 1: Configure the window
        WindowConfig config;
        config.title = "Systsem Application";
        config.width = 1024;
        config.height = 768;
        config.resizable = true;
        config.vsync = true;
        config.posX = 0;
        config.posY = 0;

        // Step 2: Create the window and input system using the factory
        // This abstracts away the platform-specific creation of Win32Window, etc.
        auto window = SystemFactory::CreateApplicationWindow(config);
        if (!window)
        {
            std::cerr << "Failed to create application window!" << std::endl;
            return -1;
        }

        // Step 3: Get the input system from the window
        // The factory ensures this is the correct and corresponding input system.
        auto input = window->GetInput();
        if (!input)
        {
            std::cerr << "Failed to get input system from window!" << std::endl;
            return -1;
        }

        // Step 4: Set up event callbacks
        window->SetResizeCallback([](int width, int height) {
            std::cout << "Window resized to: " << width << "x" << height
                      << std::endl;
        });

        window->SetCloseCallback(
            []() { std::cout << "Window close requested!" << std::endl; });

        window->SetFocusCallback([](bool hasFocus) {
            std::cout << "Window focus changed: " << (hasFocus ? "gained" : "lost")
                      << std::endl;
        });

        // Step 5: Set up input callbacks
        input->SetKeyCallback([](Key key, bool pressed) {
            if (pressed)
            {
                std::cout << "Key pressed: " << static_cast<int>(key) << std::endl;
            }
        });

        input->SetMouseButtonCallback(
            [](MouseButton button, bool pressed, int x, int y) {
                std::string buttonName = (button == MouseButton::Left)    ? "Left"
                                         : (button == MouseButton::Right) ? "Right"
                                                                          : "Middle";
                std::cout << buttonName << " mouse button "
                          << (pressed ? "pressed" : "released") << " at (" << x
                          << ", " << y << ")" << std::endl;
            });

        input->SetMouseMoveCallback([](int x, int y) {
            // Uncomment to see mouse movement (will be very verbose)
            // std::cout << "Mouse moved to: (" << x << ", " << y << ")" <<
            // std::endl;
        });

        // Step 6: Show the window
        window->Show();

        // Step 7: Main application loop
        std::cout << "Application started. Press Escape to exit." << std::endl;
        std::cout << "Controls:" << std::endl;
        std::cout << "  - WASD keys for movement input demonstration" << std::endl;
        std::cout << "  - Mouse buttons and movement for mouse input demonstration"
                  << std::endl;
        std::cout << "  - Escape to exit" << std::endl;

        bool running = true;
        while (running && !window->ShouldClose())
        {
            // Update window (processes messages and updates input)
            window->Update();

            // Example: Polling-based input checking
            if (input->IsKeyDown(Key::W))
            {
                // std::cout << "W key held down (moving forward)" << std::endl;
            }
            if (input->IsKeyDown(Key::S))
            {
                // std::cout << "S key held down (moving backward)" << std::endl;
            }
            if (input->IsKeyDown(Key::A))
            {
                // std::cout << "A key held down (moving left)" << std::endl;
            }
            if (input->IsKeyDown(Key::D))
            {
                // std::cout << "D key held down (moving right)" << std::endl;
            }

            if (input->WasKeyPressed(Key::Space))
            {
                std::cout << "Space bar was pressed this frame (jump action)"
                          << std::endl;
            }

            // Exit condition
            if (input->WasKeyPressed(Key::Escape))
            {
                running = false;
            }

// Small sleep to prevent 100% CPU usage
#ifdef _WIN32
            Sleep(1);
#else
            // On other platforms, you might use usleep
            // usleep(1000);
#endif
        }

        // Step 8: Cleanup
        // The unique_ptr for the window will handle shutdown and destruction
        // automatically when it goes out of scope.
        std::cout << "Shutting down..." << std::endl;
    }
    catch (const std::exception& e)
    {
        std::cerr << "Exception: " << e.what() << std::endl;
        return -1;
    }

    std::cout << "Application terminated successfully." << std::endl;
    return 0;
}