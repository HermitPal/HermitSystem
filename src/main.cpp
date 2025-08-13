#include "Win32Input.h"
#include "Win32Window.h"
#include <iostream>
#include <memory>

int main() {
  try {
    // Step 1: Create the input system implementation
    auto input = std::make_shared<Win32Input>();

    // Step 2: Create the window with dependency injection
    // The window constructor requires the input system
    auto window = std::make_unique<Win32Window>(input);

    // Step 3: Configure the window
    WindowConfig config;
    config.title = "Win32 Application";
    config.width = 1024;
    config.height = 768;
    config.resizable = true;
    config.vsync = true;
    config.posX = 0;
    config.posY = 0;

    // Step 4: Initialize the window (this will also initialize the input
    // system)
    if (!window->Initialize(config)) {
      std::cerr << "Failed to initialize window!" << std::endl;
      return -1;
    }

    // Step 5: Set up event callbacks
    window->SetResizeCallback([](int width, int height) {
      std::cout << "Window resized to: " << width << "x" << height << std::endl;
    });

    window->SetCloseCallback(
        []() { std::cout << "Window close requested!" << std::endl; });

    window->SetFocusCallback([](bool hasFocus) {
      std::cout << "Window focus changed: " << (hasFocus ? "gained" : "lost")
                << std::endl;
    });

    // Step 6: Set up input callbacks
    input->SetKeyCallback([](Key key, bool pressed) {
      if (pressed) {
        std::cout << "Key pressed: " << static_cast<int>(key) << std::endl;

        // Example: Exit on Escape key
        if (key == Key::Escape) {
          std::cout << "Escape pressed - requesting window close" << std::endl;
          // Note: In a real application, you'd have a way to signal the main
          // loop
        }
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
      // std::cout << "Mouse moved to: (" << x << ", " << y << ")" << std::endl;
    });

    // Step 7: Show the window
    window->Show();

    // Step 8: Main application loop
    std::cout << "Application started. Press Escape to exit." << std::endl;
    std::cout << "Controls:" << std::endl;
    std::cout << "  - WASD keys for movement input demonstration" << std::endl;
    std::cout << "  - Mouse buttons and movement for mouse input demonstration"
              << std::endl;
    std::cout << "  - Escape to exit" << std::endl;

    bool running = true;
    while (running && !window->ShouldClose()) {
      // Update window (processes messages and updates input)
      window->Update();

      // Example: Polling-based input checking
      // Check for WASD keys (common movement keys)
      if (input->IsKeyDown(Key::W)) {
        // Move forward logic would go here
        // std::cout << "W key held down (moving forward)" << std::endl;
      }
      if (input->IsKeyDown(Key::S)) {
        // Move backward logic would go here
        // std::cout << "S key held down (moving backward)" << std::endl;
      }
      if (input->IsKeyDown(Key::A)) {
        // Move left logic would go here
        // std::cout << "A key held down (moving left)" << std::endl;
      }
      if (input->IsKeyDown(Key::D)) {
        // Move right logic would go here
        // std::cout << "D key held down (moving right)" << std::endl;
      }

      // Example: Check for single-frame key presses
      if (input->WasKeyPressed(Key::Space)) {
        std::cout << "Space bar was pressed this frame (jump action)"
                  << std::endl;
      }

      // Example: Mouse input polling
      if (input->IsMouseButtonDown(MouseButton::Left)) {
        int mouseX, mouseY;
        input->GetMousePosition(mouseX, mouseY);
        // Continuous action while mouse button is held
        // std::cout << "Left mouse button held at: (" << mouseX << ", " <<
        // mouseY << ")" << std::endl;
      }

      // Check mouse delta for camera/look movement
      int deltaX, deltaY;
      input->GetMouseDelta(deltaX, deltaY);
      if (deltaX != 0 || deltaY != 0) {
        // Camera rotation logic would go here
        // std::cout << "Mouse delta: (" << deltaX << ", " << deltaY << ")" <<
        // std::endl;
      }

      // Check for mouse wheel scrolling
      int wheelDelta = input->GetMouseWheelDelta();
      if (wheelDelta != 0) {
        std::cout << "Mouse wheel scrolled: " << wheelDelta << std::endl;
      }

      // Exit condition
      if (input->WasKeyPressed(Key::Escape)) {
        running = false;
      }

      // Small sleep to prevent 100% CPU usage (in a real app, you'd have proper
      // frame timing)
      Sleep(1);
    }

    // Step 9: Cleanup
    std::cout << "Shutting down..." << std::endl;
    window->Shutdown();

  } catch (const std::exception &e) {
    std::cerr << "Exception: " << e.what() << std::endl;
    return -1;
  }

  std::cout << "Application terminated successfully." << std::endl;
  return 0;
}