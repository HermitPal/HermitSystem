#include "DirectX12Renderer.h"

#ifdef _WIN32

#include <cassert>
#include <iostream>
#include <stdexcept>


// Include DirectX 12 helper utilities
// Note: d3dx12.h provides helper utilities, but we can work without it
// #include <d3dx12.h>

// Link required DirectX libraries
#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")

namespace Renderer
{
DirectX12Renderer::DirectX12Renderer()
{
    // Initialize members are already done in header
}

DirectX12Renderer::~DirectX12Renderer()
{
    if (m_initialized)
    {
        Shutdown();
    }
}

bool DirectX12Renderer::Initialize(WindowHandle windowHandle, uint32_t width, uint32_t height)
{
    if (m_initialized)
    {
        std::cerr << "DirectX12Renderer already initialized!" << std::endl;
        return false;
    }

    m_hwnd = static_cast<HWND>(windowHandle);
    m_backBufferWidth = width;
    m_backBufferHeight = height;

    try
    {
        // Enable debug layer in debug builds
#if defined(_DEBUG)
        Microsoft::WRL::ComPtr<ID3D12Debug> debugController;
        if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController))))
        {
            debugController->EnableDebugLayer();
        }
#endif

        if (!CreateDevice())
            return false;
        if (!CreateCommandObjects())
            return false;
        if (!CreateSwapChain(windowHandle, width, height))
            return false;
        if (!CreateDescriptorHeaps())
            return false;
        if (!CreateRenderTargetViews())
            return false;
        if (!CreateDepthStencilBuffer())
            return false;

        // Set initial viewport and scissor rect
        m_screenViewport.TopLeftX = 0;
        m_screenViewport.TopLeftY = 0;
        m_screenViewport.Width = static_cast<float>(width);
        m_screenViewport.Height = static_cast<float>(height);
        m_screenViewport.MinDepth = 0.0f;
        m_screenViewport.MaxDepth = 1.0f;

        m_scissorRect = {0, 0, static_cast<int>(width), static_cast<int>(height)};

        m_initialized = true;
        std::cout << "DirectX 12 renderer initialized successfully" << std::endl;
        return true;
    }
    catch (const std::exception& e)
    {
        std::cerr << "Failed to initialize DirectX12Renderer: " << e.what() << std::endl;
        return false;
    }
}

void DirectX12Renderer::Shutdown()
{
    if (!m_initialized)
        return;

    // Wait for GPU to finish all work
    WaitForGPU();

    // Close fence event handle
    if (m_fenceEvent)
    {
        CloseHandle(m_fenceEvent);
        m_fenceEvent = nullptr;
    }

    // Release all COM objects (smart pointers will handle this automatically)
    m_initialized = false;
    std::cout << "DirectX 12 renderer shut down" << std::endl;
}

void DirectX12Renderer::BeginFrame()
{
    if (!m_initialized)
        return;

    // Reset command allocator and command list
    m_directCmdListAlloc->Reset();
    m_commandList->Reset(m_directCmdListAlloc.Get(), nullptr);

    // Indicate we're rendering to the back buffer
    D3D12_RESOURCE_BARRIER barrier = {};
    barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
    barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
    barrier.Transition.pResource = m_swapChainBuffer[m_currBackBuffer].Get();
    barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
    barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
    barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;

    m_commandList->ResourceBarrier(1, &barrier);

    // Set viewport and scissor rect
    m_commandList->RSSetViewports(1, &m_screenViewport);
    m_commandList->RSSetScissorRects(1, &m_scissorRect);
}

void DirectX12Renderer::EndFrame()
{
    if (!m_initialized)
        return;

    // Indicate we're done rendering to the back buffer
    D3D12_RESOURCE_BARRIER barrier = {};
    barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
    barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
    barrier.Transition.pResource = m_swapChainBuffer[m_currBackBuffer].Get();
    barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
    barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
    barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;

    m_commandList->ResourceBarrier(1, &barrier);

    // Close and execute command list
    m_commandList->Close();
    ID3D12CommandList* cmdLists[] = {m_commandList.Get()};
    m_commandQueue->ExecuteCommandLists(_countof(cmdLists), cmdLists);

    m_stats.frameCount++;
}

void DirectX12Renderer::Present()
{
    if (!m_initialized)
        return;

    // Present the frame
    HRESULT hr = m_swapChain->Present(1, 0); // VSync enabled
    if (FAILED(hr))
    {
        std::cerr << "Failed to present frame" << std::endl;
        return;
    }

    m_currBackBuffer = (m_currBackBuffer + 1) % SWAP_CHAIN_BUFFER_COUNT;

    // Wait for this frame to complete
    FlushCommandQueue();
}

void DirectX12Renderer::Clear(const ClearColor& color)
{
    if (!m_initialized)
        return;

    // Get current back buffer RTV
    D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = m_rtvHeap->GetCPUDescriptorHandleForHeapStart();
    rtvHandle.ptr += m_currBackBuffer * m_rtvDescriptorSize;

    D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = m_dsvHeap->GetCPUDescriptorHandleForHeapStart();

    // Set render targets
    m_commandList->OMSetRenderTargets(1, &rtvHandle, FALSE, &dsvHandle);

    // Clear render target and depth buffer
    float clearColorArray[4] = {color.r, color.g, color.b, color.a};
    m_commandList->ClearRenderTargetView(rtvHandle, clearColorArray, 0, nullptr);
    m_commandList->ClearDepthStencilView(dsvHandle,
                                         D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);
}

void DirectX12Renderer::SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height)
{
    if (!m_initialized)
        return;

    m_screenViewport.TopLeftX = static_cast<float>(x);
    m_screenViewport.TopLeftY = static_cast<float>(y);
    m_screenViewport.Width = static_cast<float>(width);
    m_screenViewport.Height = static_cast<float>(height);
    m_screenViewport.MinDepth = 0.0f;
    m_screenViewport.MaxDepth = 1.0f;

    m_scissorRect = {
        static_cast<int>(x),
        static_cast<int>(y),
        static_cast<int>(x + width),
        static_cast<int>(y + height)};

    if (m_commandList)
    {
        m_commandList->RSSetViewports(1, &m_screenViewport);
        m_commandList->RSSetScissorRects(1, &m_scissorRect);
    }
}

void DirectX12Renderer::OnResize(uint32_t width, uint32_t height)
{
    if (!m_initialized || (width == m_backBufferWidth && height == m_backBufferHeight))
        return;

    std::cout << "Resizing DirectX12 renderer to " << width << "x" << height << std::endl;

    // Wait for GPU to finish
    WaitForGPU();

    m_backBufferWidth = width;
    m_backBufferHeight = height;

    // Release old render target views
    for (int i = 0; i < SWAP_CHAIN_BUFFER_COUNT; ++i)
    {
        m_swapChainBuffer[i].Reset();
    }
    m_depthStencilBuffer.Reset();

    // Resize swap chain
    HRESULT hr = m_swapChain->ResizeBuffers(
        SWAP_CHAIN_BUFFER_COUNT,
        width,
        height,
        m_backBufferFormat,
        DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH);

    if (FAILED(hr))
    {
        std::cerr << "Failed to resize swap chain buffers" << std::endl;
        return;
    }

    m_currBackBuffer = 0;

    // Recreate render targets and depth buffer
    CreateRenderTargetViews();
    CreateDepthStencilBuffer();

    // Update viewport
    SetViewport(0, 0, width, height);
}

const char* DirectX12Renderer::GetVersion() const
{
    return "1.0"; // Simple version string
}

void DirectX12Renderer::WaitForGPU()
{
    if (!m_initialized)
        return;

    // Signal and increment the fence value
    const UINT64 fence = m_currentFence;
    m_commandQueue->Signal(m_fence.Get(), fence);
    m_currentFence++;

    // Wait until command queue is done
    if (m_fence->GetCompletedValue() < fence)
    {
        m_fence->SetEventOnCompletion(fence, m_fenceEvent);
        WaitForSingleObject(m_fenceEvent, INFINITE);
    }
}

// Dummy implementations for new IRenderer methods
BufferHandle DirectX12Renderer::CreateBuffer(BufferType type, BufferUsage usage, uint32_t size, const void* initialData)
{
    std::cout << "DirectX12Renderer: CreateBuffer (Dummy) called" << std::endl;
    return nullptr; // Dummy implementation
}

void DirectX12Renderer::DestroyBuffer(BufferHandle buffer)
{
    std::cout << "DirectX12Renderer: DestroyBuffer (Dummy) called" << std::endl;
    // Dummy implementation
}

void DirectX12Renderer::UpdateBuffer(BufferHandle buffer, uint32_t offset, uint32_t size, const void* data)
{
    std::cout << "DirectX12Renderer: UpdateBuffer (Dummy) called" << std::endl;
    // Dummy implementation
}

void DirectX12Renderer::SetVertexBuffer(BufferHandle buffer, uint32_t stride, uint32_t offset)
{
    std::cout << "DirectX12Renderer: SetVertexBuffer (Dummy) called" << std::endl;
    // Dummy implementation
}

void DirectX12Renderer::SetIndexBuffer(BufferHandle buffer, uint32_t offset)
{
    std::cout << "DirectX12Renderer: SetIndexBuffer (Dummy) called" << std::endl;
    // Dummy implementation
}

void DirectX12Renderer::SetPrimitiveTopology(PrimitiveTopology topology)
{
    std::cout << "DirectX12Renderer: SetPrimitiveTopology (Dummy) called" << std::endl;
    // Dummy implementation
}

void DirectX12Renderer::DrawIndexed(uint32_t indexCount, uint32_t startIndexLocation, int32_t baseVertexLocation)
{
    std::cout << "DirectX12Renderer: DrawIndexed (Dummy) called" << std::endl;
    // Dummy implementation
    m_stats.drawCalls++;
    m_stats.triangles += indexCount / 3; // Assuming triangle list
    m_stats.vertices += indexCount; // Assuming each index refers to a vertex
}

ShaderHandle DirectX12Renderer::CreateColorShader()
{
    std::cout << "DirectX12Renderer: CreateColorShader (Dummy) called" << std::endl;
    return nullptr; // Dummy implementation
}

void DirectX12Renderer::DestroyShader(ShaderHandle shader)
{
    std::cout << "DirectX12Renderer: DestroyShader (Dummy) called" << std::endl;
    // Dummy implementation
}

void DirectX12Renderer::SetShader(ShaderHandle shader)
{
    std::cout << "DirectX12Renderer: SetShader (Dummy) called" << std::endl;
    // Dummy implementation
}

// Private implementation methods
bool DirectX12Renderer::CreateDevice()
{
    HRESULT hr = CreateDXGIFactory1(IID_PPV_ARGS(&m_dxgiFactory));
    if (FAILED(hr))
    {
        std::cerr << "Failed to create DXGI factory" << std::endl;
        return false;
    }

    hr = D3D12CreateDevice(nullptr, D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&m_device));
    if (FAILED(hr))
    {
        std::cerr << "Failed to create D3D12 device" << std::endl;
        return false;
    }

    // Check 4X MSAA quality support
    D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS msQualityLevels;
    msQualityLevels.Format = m_backBufferFormat;
    msQualityLevels.SampleCount = 4;
    msQualityLevels.Flags = D3D12_MULTISAMPLE_QUALITY_LEVELS_FLAG_NONE;
    msQualityLevels.NumQualityLevels = 0;

    hr = m_device->CheckFeatureSupport(
        D3D12_FEATURE_MULTISAMPLE_QUALITY_LEVELS,
        &msQualityLevels,
        sizeof(msQualityLevels));

    if (SUCCEEDED(hr))
    {
        m_4xMsaaQuality = msQualityLevels.NumQualityLevels;
    }

    assert(m_4xMsaaQuality > 0 && "Unexpected MSAA quality level.");

    return true;
}

bool DirectX12Renderer::CreateCommandObjects()
{
    D3D12_COMMAND_QUEUE_DESC queueDesc = {};
    queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
    queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;

    HRESULT hr = m_device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&m_commandQueue));
    if (FAILED(hr))
    {
        std::cerr << "Failed to create command queue" << std::endl;
        return false;
    }

    hr = m_device->CreateCommandAllocator(
        D3D12_COMMAND_LIST_TYPE_DIRECT,
        IID_PPV_ARGS(m_directCmdListAlloc.GetAddressOf()));
    if (FAILED(hr))
    {
        std::cerr << "Failed to create command allocator" << std::endl;
        return false;
    }

    hr = m_device->CreateCommandList(
        0,
        D3D12_COMMAND_LIST_TYPE_DIRECT,
        m_directCmdListAlloc.Get(),
        nullptr,
        IID_PPV_ARGS(m_commandList.GetAddressOf()));
    if (FAILED(hr))
    {
        std::cerr << "Failed to create command list" << std::endl;
        return false;
    }

    // Close the command list since we'll reset it in BeginFrame
    m_commandList->Close();

    // Create fence
    hr = m_device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_fence));
    if (FAILED(hr))
    {
        std::cerr << "Failed to create fence" << std::endl;
        return false;
    }

    m_fenceEvent = CreateEventEx(nullptr, nullptr, 0, EVENT_ALL_ACCESS);
    if (m_fenceEvent == nullptr)
    {
        std::cerr << "Failed to create fence event" << std::endl;
        return false;
    }

    return true;
}

bool DirectX12Renderer::CreateSwapChain(WindowHandle windowHandle, uint32_t width, uint32_t height)
{
    DXGI_SWAP_CHAIN_DESC sd;
    sd.BufferDesc.Width = width;
    sd.BufferDesc.Height = height;
    sd.BufferDesc.RefreshRate.Numerator = 60;
    sd.BufferDesc.RefreshRate.Denominator = 1;
    sd.BufferDesc.Format = m_backBufferFormat;
    sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
    sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
    sd.SampleDesc.Count = m_4xMsaaState ? 4 : 1;
    sd.SampleDesc.Quality = m_4xMsaaState ? (m_4xMsaaQuality - 1) : 0;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.BufferCount = SWAP_CHAIN_BUFFER_COUNT;
    sd.OutputWindow = static_cast<HWND>(windowHandle);
    sd.Windowed = true;
    sd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
    sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

    Microsoft::WRL::ComPtr<IDXGISwapChain> swapChain;
    HRESULT hr = m_dxgiFactory->CreateSwapChain(
        m_commandQueue.Get(),
        &sd,
        swapChain.GetAddressOf());

    if (FAILED(hr))
    {
        std::cerr << "Failed to create swap chain" << std::endl;
        return false;
    }

    hr = swapChain.As(&m_swapChain);
    if (FAILED(hr))
    {
        std::cerr << "Failed to cast swap chain" << std::endl;
        return false;
    }

    return true;
}

bool DirectX12Renderer::CreateDescriptorHeaps()
{
    // Create RTV descriptor heap
    D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};
    rtvHeapDesc.NumDescriptors = SWAP_CHAIN_BUFFER_COUNT;
    rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
    rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
    rtvHeapDesc.NodeMask = 0;

    HRESULT hr = m_device->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(m_rtvHeap.GetAddressOf()));
    if (FAILED(hr))
    {
        std::cerr << "Failed to create RTV descriptor heap" << std::endl;
        return false;
    }

    // Create DSV descriptor heap
    D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc = {};
    dsvHeapDesc.NumDescriptors = 1;
    dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
    dsvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
    dsvHeapDesc.NodeMask = 0;

    hr = m_device->CreateDescriptorHeap(&dsvHeapDesc, IID_PPV_ARGS(m_dsvHeap.GetAddressOf()));
    if (FAILED(hr))
    {
        std::cerr << "Failed to create DSV descriptor heap" << std::endl;
        return false;
    }

    // Get descriptor sizes
    m_rtvDescriptorSize = m_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
    m_dsvDescriptorSize = m_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
    m_cbvSrvUavDescriptorSize = m_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

    return true;
}

bool DirectX12Renderer::CreateRenderTargetViews()
{
    D3D12_CPU_DESCRIPTOR_HANDLE rtvHeapHandle = m_rtvHeap->GetCPUDescriptorHandleForHeapStart();

    for (UINT i = 0; i < SWAP_CHAIN_BUFFER_COUNT; i++)
    {
        HRESULT hr = m_swapChain->GetBuffer(i, IID_PPV_ARGS(&m_swapChainBuffer[i]));
        if (FAILED(hr))
        {
            std::cerr << "Failed to get swap chain buffer " << i << std::endl;
            return false;
        }

        m_device->CreateRenderTargetView(m_swapChainBuffer[i].Get(), nullptr, rtvHeapHandle);
        rtvHeapHandle.ptr += m_rtvDescriptorSize;
    }

    return true;
}

bool DirectX12Renderer::CreateDepthStencilBuffer()
{
    // Create depth/stencil buffer description
    D3D12_RESOURCE_DESC depthStencilDesc = {};
    depthStencilDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
    depthStencilDesc.Alignment = 0;
    depthStencilDesc.Width = m_backBufferWidth;
    depthStencilDesc.Height = m_backBufferHeight;
    depthStencilDesc.DepthOrArraySize = 1;
    depthStencilDesc.MipLevels = 1;
    depthStencilDesc.Format = m_depthStencilFormat;
    depthStencilDesc.SampleDesc.Count = m_4xMsaaState ? 4 : 1;
    depthStencilDesc.SampleDesc.Quality = m_4xMsaaState ? (m_4xMsaaQuality - 1) : 0;
    depthStencilDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
    depthStencilDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

    D3D12_CLEAR_VALUE optClear = {};
    optClear.Format = m_depthStencilFormat;
    optClear.DepthStencil.Depth = 1.0f;
    optClear.DepthStencil.Stencil = 0;

    D3D12_HEAP_PROPERTIES heapProps = {};
    heapProps.Type = D3D12_HEAP_TYPE_DEFAULT;
    heapProps.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
    heapProps.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
    heapProps.CreationNodeMask = 1;
    heapProps.VisibleNodeMask = 1;

    HRESULT hr = m_device->CreateCommittedResource(
        &heapProps,
        D3D12_HEAP_FLAG_NONE,
        &depthStencilDesc,
        D3D12_RESOURCE_STATE_COMMON,
        &optClear,
        IID_PPV_ARGS(m_depthStencilBuffer.GetAddressOf()));

    if (FAILED(hr))
    {
        std::cerr << "Failed to create depth stencil buffer" << std::endl;
        return false;
    }

    // Create descriptor to mip level 0 of entire resource using format of resource
    m_device->CreateDepthStencilView(m_depthStencilBuffer.Get(), nullptr,
                                     m_dsvHeap->GetCPUDescriptorHandleForHeapStart());

    // Transition the resource from its initial state to be used as a depth buffer
    D3D12_RESOURCE_BARRIER barrier = {};
    barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
    barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
    barrier.Transition.pResource = m_depthStencilBuffer.Get();
    barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COMMON;
    barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_DEPTH_WRITE;
    barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;

    // We need to execute this transition command
    m_directCmdListAlloc->Reset();
    m_commandList->Reset(m_directCmdListAlloc.Get(), nullptr);
    m_commandList->ResourceBarrier(1, &barrier);
    m_commandList->Close();

    ID3D12CommandList* cmdLists[] = {m_commandList.Get()};
    m_commandQueue->ExecuteCommandLists(_countof(cmdLists), cmdLists);

    FlushCommandQueue();

    return true;
}

void DirectX12Renderer::FlushCommandQueue()
{
    // Advance the fence value to mark commands up to this fence point
    m_currentFence++;

    // Add an instruction to the command queue to set a new fence point
    // Because we are on the GPU timeline, the new fence point won't be
    // set until the GPU finishes processing all the commands prior to this Signal()
    HRESULT hr = m_commandQueue->Signal(m_fence.Get(), m_currentFence);
    if (FAILED(hr))
    {
        std::cerr << "Failed to signal fence" << std::endl;
        return;
    }

    // Wait until the GPU has completed commands up to this fence point
    if (m_fence->GetCompletedValue() < m_currentFence)
    {
        HANDLE eventHandle = CreateEventEx(nullptr, nullptr, 0, EVENT_ALL_ACCESS);

        // Fire event when GPU hits current fence
        hr = m_fence->SetEventOnCompletion(m_currentFence, eventHandle);
        if (FAILED(hr))
        {
            std::cerr << "Failed to set fence event" << std::endl;
            CloseHandle(eventHandle);
            return;
        }

        // Wait until the GPU hits current fence event is fired
        WaitForSingleObject(eventHandle, INFINITE);
        CloseHandle(eventHandle);
    }
}
} // namespace Renderer

#endif // _WIN32