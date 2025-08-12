#pragma once
#include "IInput.h"
#ifdef _WIN32
#include <array>
#include <windows.h>


class Win32Input : public IInput {
private:
  HWND m_hwnd = nullptr;

  std::array<bool, static_cast<size_t>(Key::Count)> m_currentKeys = {};
  std::array<bool, static_cast<size_t>(Key::Count)> m_previousKeys = {};

  MouseState m_currentMouse = {};
  MouseState m_previousMouse = {};

  std::function<void(Key, bool)> m_keyCallback;
  std::function<void(MouseButton, bool)> m_mouseButtonCallback;
  std::function<void(int, int)> m_mouseMoveCallback;

  Key VirtualKeyToKey(WPARAM vk) const;
  MouseButton VirtualButtonToMouseButton(WPARAM wParam) const;

public:
  bool Initialize(void *windowHandle) override;
  void Shutdown() override;
  void Update() override;

  bool IsKeyPressed(Key key) const override;
  bool IsKeyDown(Key key) const override;
  bool IsKeyReleased(Key key) const override;

  bool IsMouseButtonPressed(MouseButton button) const override;
  bool IsMouseButtonDown(MouseButton button) const override;
  bool IsMouseButtonReleased(MouseButton button) const override;

  MouseState GetMouseState() const override;

  void SetKeyCallback(std::function<void(Key, bool)> callback) override {
    m_keyCallback = callback;
  }
  void SetMouseButtonCallback(
      std::function<void(MouseButton, bool)> callback) override {
    m_mouseButtonCallback = callback;
  }
  void SetMouseMoveCallback(std::function<void(int, int)> callback) override {
    m_mouseMoveCallback = callback;
  }

  // Called by window message handler
  void HandleKeyEvent(WPARAM wParam, bool pressed);
  void HandleMouseEvent(WPARAM wParam, LPARAM lParam, UINT msg);
};
#endif