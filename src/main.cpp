#include "Renderer/IRenderer.h"
#include "Renderer/RendererFactory.h"
#include "System/IInput.h"
#include "System/IWindow.h"
#include "System/SystemFactory.h"
#include <iostream>
#include <memory>

// For Sleep()
#ifdef _WIN32
#include <Windows.h>
#endif

// Use the system namespace to avoid qualifying every type
using namespace Systsem;
using namespace Renderer;

int main()
{
    try
    {
        // Step 1: Configure the window
        WindowConfig config;
        config.title = "Systsem Application with Renderer";
        config.width = 1024;
        config.height = 768;
        config.resizable = true;
        config.vsync = true;
        config.posX = 0;
        config.posY = 0;

        // Step 2: Create the window and input system using the factory
        auto window = SystemFactory::CreateApplicationWindow(config);
        if (!window)
        {
            std::cerr << "Failed to create application window!" << std::endl;
            return -1;
        }

        auto input = window->GetInput();
        if (!input)
        {
            std::cerr << "Failed to get input system from window!" << std::endl;
            return -1;
        }

        // Step 3: Create and initialize the renderer
        auto renderer = RendererFactory::CreateRenderer();
        if (!renderer)
        {
            std::cerr << "Failed to create renderer!" << std::endl;
            return -1;
        }

        // Initialize the renderer with the native window handle
        if (!renderer->Initialize(window->GetNativeHandle(), config.width, config.height))
        {
            std::cerr << "Failed to initialize renderer!" << std::endl;
            return -1;
        }

        std::cout << "Renderer initialized: " << renderer->GetRendererName()
                  << " " << renderer->GetVersion() << std::endl;

        // Step 4: Set up event callbacks
        window->SetResizeCallback([&renderer](int width, int height) {
            std::cout << "Window resized to: " << width << "x" << height << std::endl;
            // Notify renderer of resize
            renderer->OnResize(static_cast<uint32_t>(width), static_cast<uint32_t>(height));
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

        // Step 6: Show the window
        window->Show();

        // Step 7: Main application loop with rendering
        std::cout << "Application started. Press Escape to exit." << std::endl;
        std::cout << "Controls:" << std::endl;
        std::cout << "  - WASD keys for movement input demonstration" << std::endl;
        std::cout << "  - Mouse buttons and movement for mouse input demonstration" << std::endl;
        std::cout << "  - R key to change clear color" << std::endl;
        std::cout << "  - Escape to exit" << std::endl;

        bool running = true;
        ClearColor clearColor = {0.2f, 0.3f, 0.4f, 1.0f}; // Nice blue-grey color
        float colorTime = 0.0f;

        while (running && !window->ShouldClose())
        {
            // Update window (processes messages and updates input)
            window->Update();

            // Example: Polling-based input checking
            if (input->IsKeyDown(Key::W))
            {
                // Move forward logic here
            }
            if (input->IsKeyDown(Key::S))
            {
                // Move backward logic here
            }
            if (input->IsKeyDown(Key::A))
            {
                // Move left logic here
            }
            if (input->IsKeyDown(Key::D))
            {
                // Move right logic here
            }

            if (input->WasKeyPressed(Key::Space))
            {
                std::cout << "Space bar was pressed this frame (jump action)" << std::endl;
            }

            // Color animation with R key
            if (input->WasKeyPressed(Key::R))
            {
                std::cout << "Changing clear color..." << std::endl;
                colorTime = 0.0f; // Reset animation
            }

            // Exit condition
            if (input->WasKeyPressed(Key::Escape))
            {
                running = false;
            }

            // Animate clear color
            colorTime += 0.016f; // Assume ~60fps
            clearColor.r = 0.5f + 0.3f * sin(colorTime);
            clearColor.g = 0.5f + 0.3f * sin(colorTime * 1.3f);
            clearColor.b = 0.5f + 0.3f * sin(colorTime * 0.7f);

            // RENDERING
            renderer->BeginFrame();

            // Clear the screen with animated color
            renderer->Clear(clearColor);

            // Set viewport to full window
            renderer->SetViewport(0, 0, renderer->GetBackBufferWidth(), renderer->GetBackBufferHeight());

            // TODO: Add your actual rendering calls here
            // renderer->DrawMesh(mesh);
            // renderer->DrawSprite(sprite);
            // etc.

            renderer->EndFrame();
            renderer->Present();

            // Print stats occasionally
            static int frameCounter = 0;
            if (++frameCounter % 300 == 0) // Every ~5 seconds at 60fps
            {
                auto stats = renderer->GetStats();
                std::cout << "Renderer Stats - Frames: " << stats.frameCount
                          << ", Frame Time: " << stats.frameTime << "ms" << std::endl;
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
        std::cout << "Shutting down renderer..." << std::endl;
        renderer->Shutdown();

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