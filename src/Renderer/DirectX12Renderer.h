#pragma once

#include "IRenderer.h"

#ifdef _WIN32
#include <d3d12.h>
#include <dxgi1_6.h>
#include <wrl/client.h>

namespace Renderer
{
class DirectX12Renderer : public IRenderer
{
  public:
    DirectX12Renderer();
    ~DirectX12Renderer() override;

    // IRenderer implementation
    bool Initialize(WindowHandle windowHandle, uint32_t width, uint32_t height) override;
    void Shutdown() override;

    void BeginFrame() override;
    void EndFrame() override;
    void Present() override;

    void Clear(const ClearColor& color = {}) override;
    void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) override;

    void OnResize(uint32_t width, uint32_t height) override;

    const char* GetRendererName() const override
    {
        return "DirectX 12";
    }
    const char* GetVersion() const override;
    RenderStats GetStats() const override
    {
        return m_stats;
    }

    bool IsInitialized() const override
    {
        return m_initialized;
    }
    uint32_t GetBackBufferWidth() const override
    {
        return m_backBufferWidth;
    }
    uint32_t GetBackBufferHeight() const override
    {
        return m_backBufferHeight;
    }

    void WaitForGPU() override;

  private:
    // DirectX 12 specific methods
    bool CreateDevice();
    bool CreateCommandObjects();
    bool CreateSwapChain(WindowHandle windowHandle, uint32_t width, uint32_t height);
    bool CreateRenderTargetViews();
    bool CreateDepthStencilBuffer();
    bool CreateDescriptorHeaps();

    void FlushCommandQueue();
    void ResetCommandList();
    void ExecuteCommandList();

    void CreateRtvAndDsvDescriptorHeaps();
    void OnResizeInternal();

    // Core DirectX 12 objects
    Microsoft::WRL::ComPtr<ID3D12Device> m_device;
    Microsoft::WRL::ComPtr<IDXGIFactory4> m_dxgiFactory;
    Microsoft::WRL::ComPtr<IDXGISwapChain> m_swapChain;

    // Command objects
    Microsoft::WRL::ComPtr<ID3D12CommandQueue> m_commandQueue;
    Microsoft::WRL::ComPtr<ID3D12CommandAllocator> m_directCmdListAlloc;
    Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> m_commandList;

    // Synchronization objects
    Microsoft::WRL::ComPtr<ID3D12Fence> m_fence;
    UINT64 m_currentFence = 0;
    HANDLE m_fenceEvent = nullptr;

    // Swap chain buffers
    static const int SWAP_CHAIN_BUFFER_COUNT = 2;
    int m_currBackBuffer = 0;
    Microsoft::WRL::ComPtr<ID3D12Resource> m_swapChainBuffer[SWAP_CHAIN_BUFFER_COUNT];
    Microsoft::WRL::ComPtr<ID3D12Resource> m_depthStencilBuffer;

    // Descriptor heaps
    Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_rtvHeap;
    Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_dsvHeap;

    // Descriptor sizes
    UINT m_rtvDescriptorSize = 0;
    UINT m_dsvDescriptorSize = 0;
    UINT m_cbvSrvUavDescriptorSize = 0;

    // Window properties
    HWND m_hwnd = nullptr;
    uint32_t m_backBufferWidth = 800;
    uint32_t m_backBufferHeight = 600;
    DXGI_FORMAT m_backBufferFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
    DXGI_FORMAT m_depthStencilFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;

    // Viewport
    D3D12_VIEWPORT m_screenViewport = {};
    D3D12_RECT m_scissorRect = {};

    // State
    bool m_initialized = false;
    bool m_4xMsaaState = false;
    UINT m_4xMsaaQuality = 0;

    // Stats tracking
    RenderStats m_stats = {};
};
} // namespace Renderer

#endif // _WIN32