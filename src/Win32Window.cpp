// Win32Window.cpp
#ifdef _WIN32
#include "Win32Window.h"
#include <iostream>

Win32Window::~Win32Window() { Shutdown(); }

bool Win32Window::Initialize(const WindowDesc &desc) {
  const wchar_t *className = L"Win32WindowClass";

  WNDCLASSEXW wc = {};
  wc.cbSize = sizeof(WNDCLASSEXW);
  wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
  wc.lpfnWndProc = WindowProc;
  wc.hInstance = GetModuleHandle(nullptr);
  wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
  wc.lpszClassName = className;

  if (!RegisterClassExW(&wc)) {
    return false;
  }

  RECT windowRect = {0, 0, desc.width, desc.height};
  AdjustWindowRectEx(&windowRect, WS_OVERLAPPEDWINDOW, FALSE, 0);

  std::wstring wTitle(desc.title.begin(), desc.title.end());
  m_hwnd = CreateWindowExW(0, className, wTitle.c_str(), WS_OVERLAPPEDWINDOW,
                           CW_USEDEFAULT, CW_USEDEFAULT,
                           windowRect.right - windowRect.left,
                           windowRect.bottom - windowRect.top, nullptr, nullptr,
                           GetModuleHandle(nullptr), this);

  if (!m_hwnd) {
    return false;
  }

  m_hdc = GetDC(m_hwnd);
  if (!CreateOpenGLContext()) {
    return false;
  }

  ShowWindow(m_hwnd, SW_SHOW);
  UpdateWindow(m_hwnd);

  return true;
}

void Win32Window::Shutdown() {
  if (m_hrc) {
    wglMakeCurrent(nullptr, nullptr);
    wglDeleteContext(m_hrc);
    m_hrc = nullptr;
  }
  if (m_hdc) {
    ReleaseDC(m_hwnd, m_hdc);
    m_hdc = nullptr;
  }
  if (m_hwnd) {
    DestroyWindow(m_hwnd);
    m_hwnd = nullptr;
  }
}

void Win32Window::SwapBuffers() {
  if (m_hdc) {
    ::SwapBuffers(m_hdc);
  }
}

void Win32Window::PollEvents() {
  MSG msg;
  while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
    TranslateMessage(&msg);
    DispatchMessage(&msg);
  }
}

void Win32Window::SetTitle(const std::string &title) {
  std::wstring wTitle(title.begin(), title.end());
  SetWindowTextW(m_hwnd, wTitle.c_str());
}

void Win32Window::SetSize(int width, int height) {
  RECT rect = {0, 0, width, height};
  AdjustWindowRectEx(&rect, GetWindowLong(m_hwnd, GWL_STYLE), FALSE, 0);
  SetWindowPos(m_hwnd, nullptr, 0, 0, rect.right - rect.left,
               rect.bottom - rect.top, SWP_NOMOVE | SWP_NOZORDER);
}

void Win32Window::GetSize(int &width, int &height) const {
  RECT rect;
  GetClientRect(m_hwnd, &rect);
  width = rect.right - rect.left;
  height = rect.bottom - rect.top;
}

LRESULT CALLBACK Win32Window::WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam,
                                         LPARAM lParam) {
  Win32Window *window = nullptr;

  if (uMsg == WM_NCCREATE) {
    CREATESTRUCT *pCreate = reinterpret_cast<CREATESTRUCT *>(lParam);
    window = reinterpret_cast<Win32Window *>(pCreate->lpCreateParams);
    SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(window));
  } else {
    window =
        reinterpret_cast<Win32Window *>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
  }

  if (window) {
    switch (uMsg) {
    case WM_CLOSE:
      window->m_shouldClose = true;
      if (window->m_closeCallback) {
        window->m_closeCallback();
      }
      return 0;

    case WM_SIZE:
      if (window->m_resizeCallback) {
        window->m_resizeCallback(LOWORD(lParam), HIWORD(lParam));
      }
      return 0;
    }
  }

  return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

bool Win32Window::CreateOpenGLContext() {
  PIXELFORMATDESCRIPTOR pfd = {};
  pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
  pfd.nVersion = 1;
  pfd.dwFlags = PFD_DOUBLEBUFFER | PFD_SUPPORT_OPENGL | PFD_DRAW_TO_WINDOW;
  pfd.iPixelType = PFD_TYPE_RGBA;
  pfd.cColorBits = 32;
  pfd.cDepthBits = 32;
  pfd.iLayerType = PFD_MAIN_PLANE;

  int pixelFormat = ChoosePixelFormat(m_hdc, &pfd);
  if (!pixelFormat || !SetPixelFormat(m_hdc, pixelFormat, &pfd)) {
    return false;
  }

  m_hrc = wglCreateContext(m_hdc);
  if (!m_hrc || !wglMakeCurrent(m_hdc, m_hrc)) {
    return false;
  }

  return true;
}
#endif