#pragma once

#include "IRenderer.h"
#include <d3d11.h>
#include <dxgi1_2.h>
#include <string>
#include <wrl/client.h>


namespace Renderer
{

class DirectX11Renderer : public IRenderer
{
  public:
    DirectX11Renderer();
    virtual ~DirectX11Renderer();

    // IRenderer interface implementation
    bool Initialize(WindowHandle windowHandle, uint32_t width, uint32_t height) override;
    void Shutdown() override;

    // Frame management
    void BeginFrame() override;
    void EndFrame() override;
    void Present() override;

    // Basic rendering operations
    void Clear(const ClearColor& color = {}) override;
    void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) override;

    // Window resize handling
    void OnResize(uint32_t width, uint32_t height) override;

    // Information getters
    const char* GetRendererName() const override;
    const char* GetVersion() const override;
    RenderStats GetStats() const override;

    // State queries
    bool IsInitialized() const override;
    uint32_t GetBackBufferWidth() const override;
    uint32_t GetBackBufferHeight() const override;

    // Resource management
    void WaitForGPU() override;

    BufferHandle CreateBuffer(BufferType type, BufferUsage usage, uint32_t size, const void* initialData = nullptr) override;
    void DestroyBuffer(BufferHandle buffer) override;
    void UpdateBuffer(BufferHandle buffer, uint32_t offset, uint32_t size, const void* data) override;

    void SetVertexBuffer(BufferHandle buffer, uint32_t stride, uint32_t offset = 0) override;
    void SetIndexBuffer(BufferHandle buffer, uint32_t offset = 0) override;
    void SetPrimitiveTopology(PrimitiveTopology topology) override;
    void DrawIndexed(uint32_t indexCount, uint32_t startIndexLocation = 0, int32_t baseVertexLocation = 0) override;

    ShaderHandle CreateColorShader() override;
    void DestroyShader(ShaderHandle shader) override;
    void SetShader(ShaderHandle shader) override;

    // DirectX 11 specific getters (for advanced usage)
    ID3D11Device* GetDevice() const
    {
        return m_device.Get();
    }
    ID3D11DeviceContext* GetDeviceContext() const
    {
        return m_deviceContext.Get();
    }
    IDXGISwapChain1* GetSwapChain() const
    {
        return m_swapChain.Get();
    }

  private:
    // Initialization helpers
    bool CreateDevice();
    bool CreateSwapChain(WindowHandle windowHandle, uint32_t width, uint32_t height);
    bool CreateRenderTargetView();
    bool CreateDepthStencilBuffer(uint32_t width, uint32_t height);
    void CleanupRenderTargets();

    // Helper functions
    void UpdateStats();
    DXGI_FORMAT GetBackBufferFormat() const
    {
        return DXGI_FORMAT_R8G8B8A8_UNORM;
    }
    DXGI_FORMAT GetDepthStencilFormat() const
    {
        return DXGI_FORMAT_D24_UNORM_S8_UINT;
    }

    // Core DirectX 11 objects
    Microsoft::WRL::ComPtr<ID3D11Device> m_device;
    Microsoft::WRL::ComPtr<ID3D11DeviceContext> m_deviceContext;
    Microsoft::WRL::ComPtr<IDXGISwapChain1> m_swapChain;
    Microsoft::WRL::ComPtr<IDXGIFactory2> m_dxgiFactory;

    // Render target resources
    Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_renderTargetView;
    Microsoft::WRL::ComPtr<ID3D11Texture2D> m_depthStencilBuffer;
    Microsoft::WRL::ComPtr<ID3D11DepthStencilView> m_depthStencilView;

    // State tracking
    bool m_initialized;
    uint32_t m_backBufferWidth;
    uint32_t m_backBufferHeight;
    WindowHandle m_windowHandle;

    // Feature level and version info
    D3D_FEATURE_LEVEL m_featureLevel;
    std::string m_rendererName;
    std::string m_version;

    // Statistics
    mutable RenderStats m_stats;
    uint64_t m_frameStartTime;

    // Synchronization
    Microsoft::WRL::ComPtr<IDXGIDevice1> m_dxgiDevice;

    // Constants
    static constexpr uint32_t FRAME_COUNT = 2; // Double buffering
};

} // namespace Renderer