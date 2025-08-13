#pragma once

#include "IRenderer.h"
#include <memory>

namespace Renderer
{
enum class RendererAPI
{
    Auto, // Automatically select best available API for platform
    DirectX11,
    DirectX12,
    Vulkan,
    OpenGL,
    Metal // For future macOS support
};

class RendererFactory
{
  public:
    // Create renderer with automatic API selection based on platform
    static RendererPtr CreateRenderer();

    // Create renderer with specific API
    static RendererPtr CreateRenderer(RendererAPI api);

    // Query available APIs on current platform
    static bool IsAPISupported(RendererAPI api);

    // Get the best available API for current platform
    static RendererAPI GetBestAvailableAPI();

    // Get human-readable name for API
    static const char* GetAPIName(RendererAPI api);

  private:
    // Platform-specific factory methods
    static RendererPtr CreateDirectX11Renderer();
    static RendererPtr CreateDirectX12Renderer();
    static RendererPtr CreateVulkanRenderer();
    static RendererPtr CreateOpenGLRenderer();
    static RendererPtr CreateMetalRenderer();

    // Platform detection helpers
    static bool IsWindows();
    static bool IsLinux();
    static bool IsMacOS();

    // API availability checks
    static bool IsDirectX11Available();
    static bool IsDirectX12Available();
    static bool IsVulkanAvailable();
    static bool IsOpenGLAvailable();
    static bool IsMetalAvailable();
};

// Convenience function for quick renderer creation
inline RendererPtr CreateDefaultRenderer()
{
    return RendererFactory::CreateRenderer();
}
} // namespace Renderer