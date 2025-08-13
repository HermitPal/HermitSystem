#pragma once

#include <cstdint>
#include <memory>

namespace Renderer
{
using WindowHandle = void*;

struct RenderStats
{
    uint64_t frameCount = 0;
    float frameTime = 0.0f;
    uint32_t drawCalls = 0;
    uint32_t vertices = 0;
    uint32_t triangles = 0;
};

struct ClearColor
{
    float r = 0.0f;
    float g = 0.0f;
    float b = 0.0f;
    float a = 1.0f;
};

class IRenderer
{
  public:
    virtual ~IRenderer() = default;

    // Initialization and cleanup
    virtual bool Initialize(WindowHandle windowHandle, uint32_t width, uint32_t height) = 0;
    virtual void Shutdown() = 0;

    // Frame management
    virtual void BeginFrame() = 0;
    virtual void EndFrame() = 0;
    virtual void Present() = 0;

    // Basic rendering operations
    virtual void Clear(const ClearColor& color = {}) = 0;
    virtual void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) = 0;

    // Window resize handling
    virtual void OnResize(uint32_t width, uint32_t height) = 0;

    // Information getters
    virtual const char* GetRendererName() const = 0;
    virtual const char* GetVersion() const = 0;
    virtual RenderStats GetStats() const = 0;

    // State queries
    virtual bool IsInitialized() const = 0;
    virtual uint32_t GetBackBufferWidth() const = 0;
    virtual uint32_t GetBackBufferHeight() const = 0;

    // Resource management (basic interface - can be extended)
    virtual void WaitForGPU() = 0;
};

using RendererPtr = std::unique_ptr<IRenderer>;
} // namespace Renderer