#include "RendererFactory.h"
#include <iostream>

#ifdef _WIN32
#include "DirectX11Renderer.h"
#include "DirectX12Renderer.h"
#include <d3d12.h>
#include <windows.h>
#include <wrl/client.h>


#endif

namespace Renderer
{
RendererPtr RendererFactory::CreateRenderer()
{
    return CreateRenderer(GetBestAvailableAPI());
}

RendererPtr RendererFactory::CreateRenderer(RendererAPI api)
{
    switch (api)
    {
    case RendererAPI::DirectX12:
        return CreateDirectX12Renderer();

    case RendererAPI::DirectX11:
        return CreateDirectX11Renderer();

    case RendererAPI::Vulkan:
        return CreateVulkanRenderer();

    case RendererAPI::OpenGL:
        return CreateOpenGLRenderer();

    case RendererAPI::Metal:
        return CreateMetalRenderer();

    case RendererAPI::Auto:
        return CreateRenderer(GetBestAvailableAPI());

    default:
        std::cerr << "Unsupported renderer API requested" << std::endl;
        return nullptr;
    }
}

bool RendererFactory::IsAPISupported(RendererAPI api)
{
    switch (api)
    {
    case RendererAPI::DirectX12:
        return IsDirectX12Available();

    case RendererAPI::DirectX11:
        return IsDirectX11Available();

    case RendererAPI::Vulkan:
        return IsVulkanAvailable();

    case RendererAPI::OpenGL:
        return IsOpenGLAvailable();

    case RendererAPI::Metal:
        return IsMetalAvailable();

    case RendererAPI::Auto:
        return true; // Auto will always find something supported

    default:
        return false;
    }
}

RendererAPI RendererFactory::GetBestAvailableAPI()
{
    // Priority order for different platforms
    if (IsWindows())
    {
        // Windows: Prefer DirectX 12, then DirectX 11, then Vulkan, then OpenGL
        // if (IsDirectX12Available())
        //     return RendererAPI::DirectX12;
        if (IsDirectX11Available())
            return RendererAPI::DirectX11;
        if (IsVulkanAvailable())
            return RendererAPI::Vulkan;
        if (IsOpenGLAvailable())
            return RendererAPI::OpenGL;
    }
    else if (IsMacOS())
    {
        // macOS: Prefer Metal, then Vulkan (via MoltenVK), then OpenGL
        if (IsMetalAvailable())
            return RendererAPI::Metal;
        if (IsVulkanAvailable())
            return RendererAPI::Vulkan;
        if (IsOpenGLAvailable())
            return RendererAPI::OpenGL;
    }
    else if (IsLinux())
    {
        // Linux: Prefer Vulkan, then OpenGL
        if (IsVulkanAvailable())
            return RendererAPI::Vulkan;
        if (IsOpenGLAvailable())
            return RendererAPI::OpenGL;
    }

    // Fallback to OpenGL as it's most widely supported
    return RendererAPI::OpenGL;
}

const char* RendererFactory::GetAPIName(RendererAPI api)
{
    switch (api)
    {
    case RendererAPI::Auto:
        return "Auto";
    case RendererAPI::DirectX11:
        return "DirectX 11";
    case RendererAPI::DirectX12:
        return "DirectX 12";
    case RendererAPI::Vulkan:
        return "Vulkan";
    case RendererAPI::OpenGL:
        return "OpenGL";
    case RendererAPI::Metal:
        return "Metal";
    default:
        return "Unknown";
    }
}

// Platform-specific factory methods
RendererPtr RendererFactory::CreateDirectX11Renderer()
{
#ifdef _WIN32
    if (!IsDirectX11Available())
    {
        std::cerr << "DirectX 11 is not available on this system" << std::endl;
        return nullptr;
    }

    try
    {
        auto renderer = std::make_unique<DirectX11Renderer>();
        std::cout << "Created DirectX 11 renderer" << std::endl;
        return renderer;
    }
    catch (const std::exception& e)
    {
        std::cerr << "Failed to create DirectX 11 renderer: " << e.what() << std::endl;
        return nullptr;
    }
#else
    std::cerr << "DirectX 11 is not available on this platform" << std::endl;
    return nullptr;
#endif
}

RendererPtr RendererFactory::CreateDirectX12Renderer()
{
#ifdef _WIN32
    if (!IsDirectX12Available())
    {
        std::cerr << "DirectX 12 is not available on this system" << std::endl;
        return nullptr;
    }

    try
    {
        auto renderer = std::make_unique<DirectX12Renderer>();
        std::cout << "Created DirectX 12 renderer" << std::endl;
        return renderer;
    }
    catch (const std::exception& e)
    {
        std::cerr << "Failed to create DirectX 12 renderer: " << e.what() << std::endl;
        return nullptr;
    }
#else
    std::cerr << "DirectX 12 is not available on this platform" << std::endl;
    return nullptr;
#endif
}

RendererPtr RendererFactory::CreateVulkanRenderer()
{
    // TODO: Implement Vulkan renderer
    std::cerr << "Vulkan renderer not implemented yet" << std::endl;
    return nullptr;
}

RendererPtr RendererFactory::CreateOpenGLRenderer()
{
    // TODO: Implement OpenGL renderer
    std::cerr << "OpenGL renderer not implemented yet" << std::endl;
    return nullptr;
}

RendererPtr RendererFactory::CreateMetalRenderer()
{
    // TODO: Implement Metal renderer
    std::cerr << "Metal renderer not implemented yet" << std::endl;
    return nullptr;
}

// Platform detection helpers
bool RendererFactory::IsWindows()
{
#ifdef _WIN32
    return true;
#else
    return false;
#endif
}

bool RendererFactory::IsLinux()
{
#ifdef __linux__
    return true;
#else
    return false;
#endif
}

bool RendererFactory::IsMacOS()
{
#ifdef __APPLE__
    return true;
#else
    return false;
#endif
}

// API availability checks
bool RendererFactory::IsDirectX11Available()
{
#ifdef _WIN32
    // DirectX 11 is available on Windows Vista and later
    // For simplicity, we'll assume it's available if we're on Windows
    return true;
#else
    return false;
#endif
}

bool RendererFactory::IsDirectX12Available()
{
#ifdef _WIN32
    // Check if DirectX 12 is available
    // This requires Windows 10 and compatible hardware
    try
    {
        Microsoft::WRL::ComPtr<ID3D12Device> testDevice;
        HRESULT hr = D3D12CreateDevice(nullptr, D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&testDevice));
        return SUCCEEDED(hr);
    }
    catch (...)
    {
        return false;
    }
#else
    return false;
#endif
}

bool RendererFactory::IsVulkanAvailable()
{
    // TODO: Check for Vulkan loader and compatible driver
    // For now, assume it's not available unless specifically implemented
    return false;
}

bool RendererFactory::IsOpenGLAvailable()
{
    // TODO: Check for OpenGL context creation capability
    // For now, assume it's available on most platforms
#if defined(_WIN32) || defined(__linux__) || defined(__APPLE__)
    return true;
#else
    return false;
#endif
}

bool RendererFactory::IsMetalAvailable()
{
#ifdef __APPLE__
    // TODO: Check for Metal framework availability
    // For now, assume it's available on macOS
    return true;
#else
    return false;
#endif
}
} // namespace Renderer