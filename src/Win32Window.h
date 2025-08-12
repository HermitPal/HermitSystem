// Win32Window.h
#pragma once
#include "IWindow.h"
#ifdef _WIN32
#include <functional>
#include <windows.h>


class Win32Window : public IWindow {
private:
  HWND m_hwnd = nullptr;
  HDC m_hdc = nullptr;
  HGLRC m_hrc = nullptr;
  bool m_shouldClose = false;

  std::function<void(int, int)> m_resizeCallback;
  std::function<void()> m_closeCallback;

  static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam,
                                     LPARAM lParam);
  bool CreateOpenGLContext();

public:
  Win32Window() = default;
  ~Win32Window() override;

  bool Initialize(const WindowDesc &desc) override;
  void Shutdown() override;
  bool ShouldClose() const override { return m_shouldClose; }
  void SwapBuffers() override;
  void PollEvents() override;

  void SetTitle(const std::string &title) override;
  void SetSize(int width, int height) override;
  void GetSize(int &width, int &height) const override;

  void *GetNativeHandle() const override { return m_hwnd; }

  void SetResizeCallback(std::function<void(int, int)> callback) override {
    m_resizeCallback = callback;
  }
  void SetCloseCallback(std::function<void()> callback) override {
    m_closeCallback = callback;
  }
};
#endif