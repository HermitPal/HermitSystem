#pragma once

#include <functional>
#include <memory>
#include <string>


// Forward declarations
class IInput;
using WindowHandle = void *;

// Window event callbacks
using WindowResizeCallback = std::function<void(int width, int height)>;
using WindowCloseCallback = std::function<void()>;
using WindowFocusCallback = std::function<void(bool hasFocus)>;

/**
 * WindowConfig - Configuration structure for window creation
 */
struct WindowConfig {
  std::string title = "Application";
  int width = 800;
  int height = 600;
  int posX = -1; // -1 means use default position
  int posY = -1; // -1 means use default position
  bool resizable = true;
  bool fullscreen = false;
  bool maximized = false;
  bool vsync = true;
  int minWidth = 320;
  int minHeight = 240;
  int maxWidth = -1;  // -1 means no limit
  int maxHeight = -1; // -1 means no limit
};

/**
 * IWindow - Abstract interface for window management
 *
 * This interface provides platform-independent window functionality.
 * It owns and manages an input system, ensuring proper initialization order.
 */
class IWindow {
public:
  virtual ~IWindow() = default;

  // Window lifecycle
  virtual bool Initialize(const WindowConfig &config) = 0;
  virtual void Shutdown() = 0;
  virtual void Update() = 0; // Process window messages and update input

  // Window properties
  virtual void SetTitle(const std::string &title) = 0;
  virtual std::string GetTitle() const = 0;

  virtual void SetSize(int width, int height) = 0;
  virtual void GetSize(int &width, int &height) const = 0;

  virtual void SetPosition(int x, int y) = 0;
  virtual void GetPosition(int &x, int &y) const = 0;

  virtual void SetMinimumSize(int minWidth, int minHeight) = 0;
  virtual void SetMaximumSize(int maxWidth, int maxHeight) = 0;

  // Window state
  virtual void Show() = 0;
  virtual void Hide() = 0;
  virtual void Minimize() = 0;
  virtual void Maximize() = 0;
  virtual void Restore() = 0;

  virtual bool IsVisible() const = 0;
  virtual bool IsMinimized() const = 0;
  virtual bool IsMaximized() const = 0;
  virtual bool HasFocus() const = 0;
  virtual bool ShouldClose() const = 0;

  // Fullscreen support
  virtual void SetFullscreen(bool fullscreen) = 0;
  virtual bool IsFullscreen() const = 0;

  // VSync control
  virtual void SetVSync(bool enabled) = 0;
  virtual bool IsVSyncEnabled() const = 0;

  // Platform-specific handle access
  virtual WindowHandle GetNativeHandle() const = 0;

  // Input system access
  virtual std::shared_ptr<IInput> GetInput() const = 0;

  // Event callbacks
  virtual void SetResizeCallback(const WindowResizeCallback &callback) = 0;
  virtual void SetCloseCallback(const WindowCloseCallback &callback) = 0;
  virtual void SetFocusCallback(const WindowFocusCallback &callback) = 0;
  virtual void ClearCallbacks() = 0;

  // Utility functions
  virtual void RequestClose() = 0;
  virtual void SetIcon(const std::string &iconPath) = 0;

  // Client area functions (excludes title bar, borders)
  virtual void GetClientSize(int &width, int &height) const = 0;
  virtual void ClientToScreen(int &x, int &y) const = 0;
  virtual void ScreenToClient(int &x, int &y) const = 0;

protected:
  // Input system reference - set during construction
  std::shared_ptr<IInput> m_input;

  // Window state
  bool m_shouldClose = false;
  bool m_isInitialized = false;
};