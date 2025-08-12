// IWindow.h
#pragma once
#include <functional>
#include <string>


struct WindowDesc {
  std::string title = "Application";
  int width = 800;
  int height = 600;
  bool resizable = true;
  bool fullscreen = false;
};

class IWindow {
public:
  virtual ~IWindow() = default;

  virtual bool Initialize(const WindowDesc &desc) = 0;
  virtual void Shutdown() = 0;
  virtual bool ShouldClose() const = 0;
  virtual void SwapBuffers() = 0;
  virtual void PollEvents() = 0;

  virtual void SetTitle(const std::string &title) = 0;
  virtual void SetSize(int width, int height) = 0;
  virtual void GetSize(int &width, int &height) const = 0;

  virtual void *GetNativeHandle() const = 0;

  // Event callbacks
  virtual void SetResizeCallback(std::function<void(int, int)> callback) = 0;
  virtual void SetCloseCallback(std::function<void()> callback) = 0;
};