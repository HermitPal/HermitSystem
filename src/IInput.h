// IInput.h
#pragma once
#include <functional>

enum class Key {
  Unknown = 0,
  A,
  B,
  C,
  D,
  E,
  F,
  G,
  H,
  I,
  J,
  K,
  L,
  M,
  N,
  O,
  P,
  Q,
  R,
  S,
  T,
  U,
  V,
  W,
  X,
  Y,
  Z,
  Num0,
  Num1,
  Num2,
  Num3,
  Num4,
  Num5,
  Num6,
  Num7,
  Num8,
  Num9,
  Space,
  Enter,
  Escape,
  Tab,
  Backspace,
  Up,
  Down,
  Left,
  Right,
  Shift,
  Ctrl,
  Alt,
  Count
};

enum class MouseButton { Left = 0, Right, Middle, Count };

struct MouseState {
  int x = 0, y = 0;
  int deltaX = 0, deltaY = 0;
  int wheel = 0;
  bool buttons[static_cast<int>(MouseButton::Count)] = {};
};

class IInput {
public:
  virtual ~IInput() = default;

  virtual bool Initialize(void *windowHandle) = 0;
  virtual void Shutdown() = 0;
  virtual void Update() = 0;

  virtual bool IsKeyPressed(Key key) const = 0;
  virtual bool IsKeyDown(Key key) const = 0;
  virtual bool IsKeyReleased(Key key) const = 0;

  virtual bool IsMouseButtonPressed(MouseButton button) const = 0;
  virtual bool IsMouseButtonDown(MouseButton button) const = 0;
  virtual bool IsMouseButtonReleased(MouseButton button) const = 0;

  virtual MouseState GetMouseState() const = 0;

  // Event callbacks
  virtual void SetKeyCallback(std::function<void(Key, bool)> callback) = 0;
  virtual void
  SetMouseButtonCallback(std::function<void(MouseButton, bool)> callback) = 0;
  virtual void SetMouseMoveCallback(std::function<void(int, int)> callback) = 0;
};
