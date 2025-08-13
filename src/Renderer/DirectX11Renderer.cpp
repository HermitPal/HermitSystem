#include "DirectX11Renderer.h"
#include <chrono>
#include <iostream>
#include <sstream>


using Microsoft::WRL::ComPtr;

namespace Renderer
{

DirectX11Renderer::DirectX11Renderer()
    : m_initialized(false), m_backBufferWidth(0), m_backBufferHeight(0), m_windowHandle(nullptr), m_featureLevel(D3D_FEATURE_LEVEL_11_0), m_frameStartTime(0)
{
    m_rendererName = "DirectX 11 Renderer";
    m_version = "DirectX 11.0";
}

DirectX11Renderer::~DirectX11Renderer()
{
    Shutdown();
}

bool DirectX11Renderer::Initialize(WindowHandle windowHandle, uint32_t width, uint32_t height)
{
    if (m_initialized)
    {
        std::cout << "DirectX11Renderer: Already initialized" << std::endl;
        return true;
    }

    if (!windowHandle)
    {
        std::cout << "DirectX11Renderer: Invalid window handle" << std::endl;
        return false;
    }

    if (width == 0 || height == 0)
    {
        std::cout << "DirectX11Renderer: Invalid dimensions" << std::endl;
        return false;
    }

    m_windowHandle = windowHandle;
    m_backBufferWidth = width;
    m_backBufferHeight = height;

    // Initialize DirectX 11 components
    if (!CreateDevice())
    {
        std::cout << "DirectX11Renderer: Failed to create device" << std::endl;
        return false;
    }

    if (!CreateSwapChain(windowHandle, width, height))
    {
        std::cout << "DirectX11Renderer: Failed to create swap chain" << std::endl;
        return false;
    }

    if (!CreateRenderTargetView())
    {
        std::cout << "DirectX11Renderer: Failed to create render target view" << std::endl;
        return false;
    }

    if (!CreateDepthStencilBuffer(width, height))
    {
        std::cout << "DirectX11Renderer: Failed to create depth stencil buffer" << std::endl;
        return false;
    }

    // Set initial viewport
    SetViewport(0, 0, width, height);

    // Set render targets
    m_deviceContext->OMSetRenderTargets(1, m_renderTargetView.GetAddressOf(), m_depthStencilView.Get());

    // Update version string with actual feature level
    std::ostringstream oss;
    oss << "DirectX 11." << ((m_featureLevel >> 12) & 0xF) << "_" << ((m_featureLevel >> 8) & 0xF);
    m_version = oss.str();

    m_initialized = true;
    std::cout << "DirectX11Renderer: Successfully initialized (" << m_version << ")" << std::endl;

    return true;
}

void DirectX11Renderer::Shutdown()
{
    if (!m_initialized)
        return;

    WaitForGPU();

    CleanupRenderTargets();

    m_swapChain.Reset();
    m_dxgiFactory.Reset();
    m_dxgiDevice.Reset();
    m_deviceContext.Reset();
    m_device.Reset();

    m_initialized = false;
    m_windowHandle = nullptr;

    std::cout << "DirectX11Renderer: Shutdown complete" << std::endl;
}

void DirectX11Renderer::BeginFrame()
{
    if (!m_initialized)
        return;

    m_frameStartTime = std::chrono::duration_cast<std::chrono::microseconds>(
                           std::chrono::high_resolution_clock::now().time_since_epoch())
                           .count();

    // Clear stats for this frame
    m_stats.drawCalls = 0;
    m_stats.vertices = 0;
    m_stats.triangles = 0;
}

void DirectX11Renderer::EndFrame()
{
    if (!m_initialized)
        return;

    UpdateStats();
}

void DirectX11Renderer::Present()
{
    if (!m_initialized)
        return;

    // Present with VSync (1) - can be made configurable
    HRESULT hr = m_swapChain->Present(1, 0);

    if (FAILED(hr))
    {
        if (hr == DXGI_ERROR_DEVICE_REMOVED || hr == DXGI_ERROR_DEVICE_RESET)
        {
            std::cout << "DirectX11Renderer: Device lost during present" << std::endl;
        }
    }

    m_stats.frameCount++;
}

void DirectX11Renderer::Clear(const ClearColor& color)
{
    if (!m_initialized)
        return;

    float clearColor[4] = {color.r, color.g, color.b, color.a};
    m_deviceContext->ClearRenderTargetView(m_renderTargetView.Get(), clearColor);
    m_deviceContext->ClearDepthStencilView(m_depthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
}

void DirectX11Renderer::SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height)
{
    if (!m_initialized)
        return;

    D3D11_VIEWPORT viewport = {};
    viewport.TopLeftX = static_cast<float>(x);
    viewport.TopLeftY = static_cast<float>(y);
    viewport.Width = static_cast<float>(width);
    viewport.Height = static_cast<float>(height);
    viewport.MinDepth = 0.0f;
    viewport.MaxDepth = 1.0f;

    m_deviceContext->RSSetViewports(1, &viewport);
}

void DirectX11Renderer::OnResize(uint32_t width, uint32_t height)
{
    if (!m_initialized || (width == m_backBufferWidth && height == m_backBufferHeight))
        return;

    if (width == 0 || height == 0)
        return;

    WaitForGPU();

    // Clean up render target views
    CleanupRenderTargets();

    // Resize swap chain buffers
    HRESULT hr = m_swapChain->ResizeBuffers(FRAME_COUNT, width, height, GetBackBufferFormat(), 0);
    if (FAILED(hr))
    {
        std::cout << "DirectX11Renderer: Failed to resize swap chain buffers" << std::endl;
        return;
    }

    m_backBufferWidth = width;
    m_backBufferHeight = height;

    // Recreate render targets
    if (!CreateRenderTargetView())
    {
        std::cout << "DirectX11Renderer: Failed to recreate render target view after resize" << std::endl;
        return;
    }

    if (!CreateDepthStencilBuffer(width, height))
    {
        std::cout << "DirectX11Renderer: Failed to recreate depth stencil buffer after resize" << std::endl;
        return;
    }

    // Update viewport
    SetViewport(0, 0, width, height);

    // Set render targets
    m_deviceContext->OMSetRenderTargets(1, m_renderTargetView.GetAddressOf(), m_depthStencilView.Get());

    std::cout << "DirectX11Renderer: Resized to " << width << "x" << height << std::endl;
}

const char* DirectX11Renderer::GetRendererName() const
{
    return m_rendererName.c_str();
}

const char* DirectX11Renderer::GetVersion() const
{
    return m_version.c_str();
}

RenderStats DirectX11Renderer::GetStats() const
{
    return m_stats;
}

bool DirectX11Renderer::IsInitialized() const
{
    return m_initialized;
}

uint32_t DirectX11Renderer::GetBackBufferWidth() const
{
    return m_backBufferWidth;
}

uint32_t DirectX11Renderer::GetBackBufferHeight() const
{
    return m_backBufferHeight;
}

void DirectX11Renderer::WaitForGPU()
{
    if (!m_initialized)
        return;

    // Flush the immediate context
    m_deviceContext->Flush();

    // For DirectX 11, we don't have explicit GPU synchronization like DX12
    // The flush ensures all commands are submitted to the GPU
}

// Dummy implementations for new IRenderer methods
BufferHandle DirectX11Renderer::CreateBuffer(BufferType type, BufferUsage usage, uint32_t size, const void* initialData)
{
    std::cout << "DirectX11Renderer: CreateBuffer (Dummy) called" << std::endl;
    return nullptr; // Dummy implementation
}

void DirectX11Renderer::DestroyBuffer(BufferHandle buffer)
{
    std::cout << "DirectX11Renderer: DestroyBuffer (Dummy) called" << std::endl;
    // Dummy implementation
}

void DirectX11Renderer::UpdateBuffer(BufferHandle buffer, uint32_t offset, uint32_t size, const void* data)
{
    std::cout << "DirectX11Renderer: UpdateBuffer (Dummy) called" << std::endl;
    // Dummy implementation
}

void DirectX11Renderer::SetVertexBuffer(BufferHandle buffer, uint32_t stride, uint32_t offset)
{
    std::cout << "DirectX11Renderer: SetVertexBuffer (Dummy) called" << std::endl;
    // Dummy implementation
}

void DirectX11Renderer::SetIndexBuffer(BufferHandle buffer, uint32_t offset)
{
    std::cout << "DirectX11Renderer: SetIndexBuffer (Dummy) called" << std::endl;
    // Dummy implementation
}

void DirectX11Renderer::SetPrimitiveTopology(PrimitiveTopology topology)
{
    std::cout << "DirectX11Renderer: SetPrimitiveTopology (Dummy) called" << std::endl;
    // Dummy implementation
}

void DirectX11Renderer::DrawIndexed(uint32_t indexCount, uint32_t startIndexLocation, int32_t baseVertexLocation)
{
    std::cout << "DirectX11Renderer: DrawIndexed (Dummy) called" << std::endl;
    // Dummy implementation
    m_stats.drawCalls++;
    m_stats.triangles += indexCount / 3; // Assuming triangle list
    m_stats.vertices += indexCount; // Assuming each index refers to a vertex
}

ShaderHandle DirectX11Renderer::CreateColorShader()
{
    std::cout << "DirectX11Renderer: CreateColorShader (Dummy) called" << std::endl;
    return nullptr; // Dummy implementation
}

void DirectX11Renderer::DestroyShader(ShaderHandle shader)
{
    std::cout << "DirectX11Renderer: DestroyShader (Dummy) called" << std::endl;
    // Dummy implementation
}

void DirectX11Renderer::SetShader(ShaderHandle shader)
{
    std::cout << "DirectX11Renderer: SetShader (Dummy) called" << std::endl;
    // Dummy implementation
}

// Private implementation methods

bool DirectX11Renderer::CreateDevice()
{
    UINT createDeviceFlags = 0;
#ifdef _DEBUG
    createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

    D3D_FEATURE_LEVEL featureLevels[] = {
        D3D_FEATURE_LEVEL_11_1,
        D3D_FEATURE_LEVEL_11_0,
        D3D_FEATURE_LEVEL_10_1,
        D3D_FEATURE_LEVEL_10_0};

    HRESULT hr = D3D11CreateDevice(
        nullptr, // Use default adapter
        D3D_DRIVER_TYPE_HARDWARE,
        nullptr, // No software module
        createDeviceFlags,
        featureLevels,
        ARRAYSIZE(featureLevels),
        D3D11_SDK_VERSION,
        &m_device,
        &m_featureLevel,
        &m_deviceContext);

    if (FAILED(hr))
    {
        std::cout << "DirectX11Renderer: Failed to create D3D11 device" << std::endl;
        return false;
    }

    // Get DXGI device for swap chain creation
    hr = m_device.As(&m_dxgiDevice);
    if (FAILED(hr))
    {
        std::cout << "DirectX11Renderer: Failed to get DXGI device" << std::endl;
        return false;
    }

    return true;
}

bool DirectX11Renderer::CreateSwapChain(WindowHandle windowHandle, uint32_t width, uint32_t height)
{
    // Get DXGI adapter and factory
    ComPtr<IDXGIAdapter> adapter;
    HRESULT hr = m_dxgiDevice->GetAdapter(&adapter);
    if (FAILED(hr))
    {
        std::cout << "DirectX11Renderer: Failed to get DXGI adapter" << std::endl;
        return false;
    }

    hr = adapter->GetParent(IID_PPV_ARGS(&m_dxgiFactory));
    if (FAILED(hr))
    {
        std::cout << "DirectX11Renderer: Failed to get DXGI factory" << std::endl;
        return false;
    }

    // Create swap chain description
    DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
    swapChainDesc.Width = width;
    swapChainDesc.Height = height;
    swapChainDesc.Format = GetBackBufferFormat();
    swapChainDesc.Stereo = FALSE;
    swapChainDesc.SampleDesc.Count = 1;
    swapChainDesc.SampleDesc.Quality = 0;
    swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapChainDesc.BufferCount = FRAME_COUNT;
    swapChainDesc.Scaling = DXGI_SCALING_STRETCH;
    swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
    swapChainDesc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
    swapChainDesc.Flags = 0;

    hr = m_dxgiFactory->CreateSwapChainForHwnd(
        m_device.Get(),
        static_cast<HWND>(windowHandle),
        &swapChainDesc,
        nullptr,
        nullptr,
        &m_swapChain);

    if (FAILED(hr))
    {
        std::cout << "DirectX11Renderer: Failed to create swap chain" << std::endl;
        return false;
    }

    // Disable Alt+Enter fullscreen toggle
    m_dxgiFactory->MakeWindowAssociation(static_cast<HWND>(windowHandle), DXGI_MWA_NO_ALT_ENTER);

    return true;
}

bool DirectX11Renderer::CreateRenderTargetView()
{
    ComPtr<ID3D11Texture2D> backBuffer;
    HRESULT hr = m_swapChain->GetBuffer(0, IID_PPV_ARGS(&backBuffer));
    if (FAILED(hr))
    {
        std::cout << "DirectX11Renderer: Failed to get back buffer" << std::endl;
        return false;
    }

    hr = m_device->CreateRenderTargetView(backBuffer.Get(), nullptr, &m_renderTargetView);
    if (FAILED(hr))
    {
        std::cout << "DirectX11Renderer: Failed to create render target view" << std::endl;
        return false;
    }

    return true;
}

bool DirectX11Renderer::CreateDepthStencilBuffer(uint32_t width, uint32_t height)
{
    // Create depth stencil texture
    D3D11_TEXTURE2D_DESC depthStencilDesc = {};
    depthStencilDesc.Width = width;
    depthStencilDesc.Height = height;
    depthStencilDesc.MipLevels = 1;
    depthStencilDesc.ArraySize = 1;
    depthStencilDesc.Format = GetDepthStencilFormat();
    depthStencilDesc.SampleDesc.Count = 1;
    depthStencilDesc.SampleDesc.Quality = 0;
    depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
    depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    depthStencilDesc.CPUAccessFlags = 0;
    depthStencilDesc.MiscFlags = 0;

    HRESULT hr = m_device->CreateTexture2D(&depthStencilDesc, nullptr, &m_depthStencilBuffer);
    if (FAILED(hr))
    {
        std::cout << "DirectX11Renderer: Failed to create depth stencil texture" << std::endl;
        return false;
    }

    // Create depth stencil view
    D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc = {};
    depthStencilViewDesc.Format = GetDepthStencilFormat();
    depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
    depthStencilViewDesc.Texture2D.MipSlice = 0;

    hr = m_device->CreateDepthStencilView(m_depthStencilBuffer.Get(), &depthStencilViewDesc, &m_depthStencilView);
    if (FAILED(hr))
    {
        std::cout << "DirectX11Renderer: Failed to create depth stencil view" << std::endl;
        return false;
    }

    return true;
}

void DirectX11Renderer::CleanupRenderTargets()
{
    m_deviceContext->OMSetRenderTargets(0, nullptr, nullptr);
    m_renderTargetView.Reset();
    m_depthStencilView.Reset();
    m_depthStencilBuffer.Reset();
}

void DirectX11Renderer::UpdateStats()
{
    uint64_t frameEndTime = std::chrono::duration_cast<std::chrono::microseconds>(
                                std::chrono::high_resolution_clock::now().time_since_epoch())
                                .count();

    m_stats.frameTime = (frameEndTime - m_frameStartTime) / 1000.0f; // Convert to milliseconds
}

} // namespace Renderer