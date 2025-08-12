#ifdef _WIN32
#include "Win32Input.h"

bool Win32Input::Initialize(void *windowHandle) {
  m_hwnd = static_cast<HWND>(windowHandle);
  return m_hwnd != nullptr;
}

void Win32Input::Shutdown() { m_hwnd = nullptr; }

void Win32Input::Update() {
  // Store previous state
  m_previousKeys = m_currentKeys;
  m_previousMouse = m_currentMouse;

  // Update current state
  POINT cursor;
  GetCursorPos(&cursor);
  ScreenToClient(m_hwnd, &cursor);

  m_currentMouse.deltaX = cursor.x - m_currentMouse.x;
  m_currentMouse.deltaY = cursor.y - m_currentMouse.y;
  m_currentMouse.x = cursor.x;
  m_currentMouse.y = cursor.y;

  // Update key states
  for (size_t i = 0; i < static_cast<size_t>(Key::Count); ++i) {
    // This would need proper virtual key mapping
  }
}

bool Win32Input::IsKeyPressed(Key key) const {
  size_t index = static_cast<size_t>(key);
  return m_currentKeys[index] && !m_previousKeys[index];
}

bool Win32Input::IsKeyDown(Key key) const {
  return m_currentKeys[static_cast<size_t>(key)];
}

bool Win32Input::IsKeyReleased(Key key) const {
  size_t index = static_cast<size_t>(key);
  return !m_currentKeys[index] && m_previousKeys[index];
}

bool Win32Input::IsMouseButtonPressed(MouseButton button) const {
  size_t index = static_cast<size_t>(button);
  return m_currentMouse.buttons[index] && !m_previousMouse.buttons[index];
}

bool Win32Input::IsMouseButtonDown(MouseButton button) const {
  return m_currentMouse.buttons[static_cast<size_t>(button)];
}

bool Win32Input::IsMouseButtonReleased(MouseButton button) const {
  size_t index = static_cast<size_t>(button);
  return !m_currentMouse.buttons[index] && m_previousMouse.buttons[index];
}

MouseState Win32Input::GetMouseState() const { return m_currentMouse; }

void Win32Input::HandleKeyEvent(WPARAM wParam, bool pressed) {
  Key key = VirtualKeyToKey(wParam);
  if (key != Key::Unknown) {
    m_currentKeys[static_cast<size_t>(key)] = pressed;
    if (m_keyCallback) {
      m_keyCallback(key, pressed);
    }
  }
}

void Win32Input::HandleMouseEvent(WPARAM wParam, LPARAM lParam, UINT msg) {
  switch (msg) {
  case WM_LBUTTONDOWN:
  case WM_LBUTTONUP:
    m_currentMouse.buttons[static_cast<size_t>(MouseButton::Left)] =
        (msg == WM_LBUTTONDOWN);
    break;
  case WM_RBUTTONDOWN:
  case WM_RBUTTONUP:
    m_currentMouse.buttons[static_cast<size_t>(MouseButton::Right)] =
        (msg == WM_RBUTTONDOWN);
    break;
  case WM_MBUTTONDOWN:
  case WM_MBUTTONUP:
    m_currentMouse.buttons[static_cast<size_t>(MouseButton::Middle)] =
        (msg == WM_MBUTTONDOWN);
    break;
  case WM_MOUSEWHEEL:
    m_currentMouse.wheel += GET_WHEEL_DELTA_WPARAM(wParam) / WHEEL_DELTA;
    break;
  }
}

Key Win32Input::VirtualKeyToKey(WPARAM vk) const {
  switch (vk) {
  case 'A':
    return Key::A;
  case 'B':
    return Key::B;
  case 'C':
    return Key::C;
  case 'D':
    return Key::D;
  case 'E':
    return Key::E;
  case 'F':
    return Key::F;
  case 'G':
    return Key::G;
  case 'H':
    return Key::H;
  case 'I':
    return Key::I;
  case 'J':
    return Key::J;
  case 'K':
    return Key::K;
  case 'L':
    return Key::L;
  case 'M':
    return Key::M;
  case 'N':
    return Key::N;
  case 'O':
    return Key::O;
  case 'P':
    return Key::P;
  case 'Q':
    return Key::Q;
  case 'R':
    return Key::R;
  case 'S':
    return Key::S;
  case 'T':
    return Key::T;
  case 'U':
    return Key::U;
  case 'V':
    return Key::V;
  case 'W':
    return Key::W;
  case 'X':
    return Key::X;
  case 'Y':
    return Key::Y;
  case 'Z':
    return Key::Z;
  case '0':
    return Key::Num0;
  case '1':
    return Key::Num1;
  case '2':
    return Key::Num2;
  case '3':
    return Key::Num3;
  case '4':
    return Key::Num4;
  case '5':
    return Key::Num5;
  case '6':
    return Key::Num6;
  case '7':
    return Key::Num7;
  case '8':
    return Key::Num8;
  case '9':
    return Key::Num9;
  case VK_SPACE:
    return Key::Space;
  case VK_RETURN:
    return Key::Enter;
  case VK_ESCAPE:
    return Key::Escape;
  case VK_TAB:
    return Key::Tab;
  case VK_BACK:
    return Key::Backspace;
  case VK_UP:
    return Key::Up;
  case VK_DOWN:
    return Key::Down;
  case VK_LEFT:
    return Key::Left;
  case VK_RIGHT:
    return Key::Right;
  case VK_SHIFT:
    return Key::Shift;
  case VK_CONTROL:
    return Key::Ctrl;
  case VK_MENU:
    return Key::Alt;
  default:
    return Key::Unknown;
  }
}
#endif