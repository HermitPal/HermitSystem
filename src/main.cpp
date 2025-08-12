// main.cpp - Example Usage
#include <iostream>
#include <memory>

#ifdef _WIN32
#include "Win32Input.h"
#include "Win32Window.h"

#else
// Include other platform implementations
#endif

std::unique_ptr<IWindow> CreateWindowKek() {
#ifdef _WIN32
  return std::make_unique<Win32Window>();
#else
  // Return other platform implementations
  return nullptr;
#endif
}

std::unique_ptr<IInput> CreateInput() {
#ifdef _WIN32
  return std::make_unique<Win32Input>();
#else
  // Return other platform implementations
  return nullptr;
#endif
}

int main() {
  // Create platform-specific implementations
  auto window = CreateWindowKek();
  auto input = CreateInput();

  if (!window || !input) {
    std::cerr << "Failed to create window or input system\n";
    return -1;
  }

  // Initialize window
  WindowDesc desc;
  desc.title = "Cross-Platform Application";
  desc.width = 1024;
  desc.height = 768;

  if (!window->Initialize(desc)) {
    std::cerr << "Failed to initialize window\n";
    return -1;
  }

  // Initialize input with window handle
  if (!input->Initialize(window->GetNativeHandle())) {
    std::cerr << "Failed to initialize input\n";
    return -1;
  }

  // Set up callbacks
  window->SetResizeCallback([](int width, int height) {
    std::cout << "Window resized: " << width << "x" << height << "\n";
  });

  input->SetKeyCallback([](Key key, bool pressed) {
    if (pressed) {
      std::cout << "Key pressed: " << static_cast<int>(key) << "\n";
    }
  });

  // Main loop
  while (!window->ShouldClose()) {
    window->PollEvents();
    input->Update();

    // Check for escape key to quit
    if (input->IsKeyPressed(Key::Escape)) {
      break;
    }

    // Example input handling
    if (input->IsKeyDown(Key::W)) {
      std::cout << "W is being held down\n";
    }

    if (input->IsMouseButtonPressed(MouseButton::Left)) {
      MouseState mouse = input->GetMouseState();
      std::cout << "Left mouse clicked at: " << mouse.x << ", " << mouse.y
                << "\n";
    }

    // Render here...

    window->SwapBuffers();
  }

  // Cleanup
  input->Shutdown();
  window->Shutdown();

  return 0;
}