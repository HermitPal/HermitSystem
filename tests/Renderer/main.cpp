#include "Renderer/IRenderer.h"
#include "Renderer/RendererFactory.h"
#include "System/IWindow.h"
#include "System/SystemFactory.h"
#include "Renderer/RendererResources.h" // Added this include
#include <gtest/gtest.h>
#include <memory>

using namespace Renderer;
using namespace System;

// Test fixture for Renderer tests
class RendererTest : public ::testing::Test
{
  protected:
    void SetUp() override
    {
        // Set up a window for the renderer to use
        WindowConfig config;
        config.title = "Test Renderer Window";
        config.width = 800;
        config.height = 600;
        config.vsync = false;

        window = SystemFactory::CreateApplicationWindow(config);
        ASSERT_NE(window, nullptr);

        // The factory already initializes the window, so no need to call Initialize() again
    }

    void TearDown() override
    {
        if (renderer)
        {
            renderer->Shutdown();
            renderer.reset();
        }
        if (window)
        {
            window->Shutdown();
            window.reset();
        }
    }

    std::unique_ptr<IWindow> window;
    std::unique_ptr<IRenderer> renderer;
};

// Test that the factory can create a renderer
TEST_F(RendererTest, FactoryCreatesRenderer)
{
    renderer = RendererFactory::CreateRenderer();
    ASSERT_NE(renderer, nullptr) << "Factory should create a valid renderer";
}

// Test that the renderer can be initialized
TEST_F(RendererTest, RendererInitialization)
{
    renderer = RendererFactory::CreateRenderer();
    ASSERT_NE(renderer, nullptr);

    // Get window dimensions using the correct interface method
    int width, height;
    window->GetSize(width, height);

    bool success = renderer->Initialize(window->GetNativeHandle(), width, height);
    ASSERT_TRUE(success) << "Renderer should initialize successfully";

    EXPECT_NE(renderer->GetRendererName(), "Unknown") << "Renderer name should be set after initialization";
}

// Test that renderer initialization fails with invalid parameters
TEST_F(RendererTest, RendererInitializationWithInvalidParams)
{
    renderer = RendererFactory::CreateRenderer();
    ASSERT_NE(renderer, nullptr);

    // Test with null window handle
    bool success = renderer->Initialize(nullptr, 800, 600);
    EXPECT_FALSE(success) << "Renderer should fail to initialize with null window handle";
}

// Test a single frame cycle
TEST_F(RendererTest, RendererFrameCycle)
{
    renderer = RendererFactory::CreateRenderer();
    ASSERT_NE(renderer, nullptr);

    int width, height;
    window->GetSize(width, height);

    ASSERT_TRUE(renderer->Initialize(window->GetNativeHandle(), width, height));

    // Run a few frames to ensure no crashes
    for (int i = 0; i < 5; ++i)
    {
        ASSERT_NO_THROW({
            renderer->BeginFrame();
            renderer->Clear({0.1f, 0.1f, 0.1f, 1.0f});
            renderer->EndFrame();
            renderer->Present();
        }) << "Render cycle should not throw exceptions on frame "
           << i;
    }
}

// Test renderer with window resize
TEST_F(RendererTest, RendererHandlesWindowResize)
{
    renderer = RendererFactory::CreateRenderer();
    ASSERT_NE(renderer, nullptr);

    int width, height;
    window->GetSize(width, height);

    ASSERT_TRUE(renderer->Initialize(window->GetNativeHandle(), width, height));

    // Resize the window
    int newWidth = 1024, newHeight = 768;
    window->SetSize(newWidth, newHeight);

    // Update window to process the resize
    window->Update();

    // Get the actual new size (might be different due to platform constraints)
    window->GetSize(width, height);

    // Test that renderer can handle the new size
    ASSERT_NO_THROW({
        renderer->BeginFrame();
        renderer->Clear({0.2f, 0.2f, 0.2f, 1.0f});
        renderer->EndFrame();
        renderer->Present();
    }) << "Renderer should handle window resize gracefully";
}

// Test renderer shutdown and cleanup
TEST_F(RendererTest, RendererShutdown)
{
    renderer = RendererFactory::CreateRenderer();
    ASSERT_NE(renderer, nullptr);

    int width, height;
    window->GetSize(width, height);

    ASSERT_TRUE(renderer->Initialize(window->GetNativeHandle(), width, height));

    // Should be able to shutdown cleanly
    ASSERT_NO_THROW({
        renderer->Shutdown();
    }) << "Renderer shutdown should not throw exceptions";

    // Should be safe to call shutdown multiple times
    ASSERT_NO_THROW({
        renderer->Shutdown();
    }) << "Multiple shutdown calls should be safe";
}

// Test renderer properties after initialization
TEST_F(RendererTest, RendererProperties)
{
    renderer = RendererFactory::CreateRenderer();
    ASSERT_NE(renderer, nullptr);

    int width, height;
    window->GetSize(width, height);

    ASSERT_TRUE(renderer->Initialize(window->GetNativeHandle(), width, height));

    // Check that renderer name is not empty or unknown
    std::string rendererName = renderer->GetRendererName();
    EXPECT_FALSE(rendererName.empty()) << "Renderer name should not be empty";
    EXPECT_NE(rendererName, "Unknown") << "Renderer name should be properly set";

    // You might want to add more property checks here based on your renderer interface
    // For example, if you have methods like GetAPIVersion(), GetMaxTextureSize(), etc.
}

// Test multiple renderers (if supported)
TEST_F(RendererTest, MultipleRendererInstances)
{
    auto renderer1 = RendererFactory::CreateRenderer();
    auto renderer2 = RendererFactory::CreateRenderer();

    ASSERT_NE(renderer1, nullptr);
    ASSERT_NE(renderer2, nullptr);

    // Both should be valid but separate instances
    EXPECT_NE(renderer1.get(), renderer2.get()) << "Factory should create separate instances";

    // Clean up
    if (renderer1)
        renderer1->Shutdown();
    if (renderer2)
        renderer2->Shutdown();
}

// Test for drawing a simple triangle (Hello Triangle)
TEST_F(RendererTest, HelloTriangle)
{
    renderer = RendererFactory::CreateRenderer();
    ASSERT_NE(renderer, nullptr);

    int width, height;
    window->GetSize(width, height);

    ASSERT_TRUE(renderer->Initialize(window->GetNativeHandle(), width, height));

    // Define triangle vertices (CPU-side)
    std::vector<Vertex> vertices = {
        {{0.0f, 0.5f, 0.0f}, {1.0f, 0.0f, 0.0f, 1.0f}},   // Top (Red)
        {{0.5f, -0.5f, 0.0f}, {0.0f, 1.0f, 0.0f, 1.0f}},  // Right (Green)
        {{-0.5f, -0.5f, 0.0f}, {0.0f, 0.0f, 1.0f, 1.0f}} // Left (Blue)
    };

    // Define triangle indices
    std::vector<uint32_t> indices = {0, 1, 2};

    // Create vertex buffer
    BufferHandle vertexBuffer = renderer->CreateBuffer(BufferType::VertexBuffer, BufferUsage::Immutable, sizeof(Vertex) * vertices.size(), vertices.data());
    ASSERT_NE(vertexBuffer, nullptr) << "Failed to create vertex buffer";

    // Create index buffer
    BufferHandle indexBuffer = renderer->CreateBuffer(BufferType::IndexBuffer, BufferUsage::Immutable, sizeof(uint32_t) * indices.size(), indices.data());
    ASSERT_NE(indexBuffer, nullptr) << "Failed to create index buffer";

    // Create a simple color shader
    ShaderHandle colorShader = renderer->CreateColorShader();
    ASSERT_NE(colorShader, nullptr) << "Failed to create color shader";

    // Render a single frame with the triangle
    ASSERT_NO_THROW({
        renderer->BeginFrame();
        renderer->Clear({0.1f, 0.1f, 0.1f, 1.0f});
        renderer->SetViewport(0, 0, renderer->GetBackBufferWidth(), renderer->GetBackBufferHeight());

        renderer->SetShader(colorShader);
        renderer->SetVertexBuffer(vertexBuffer, sizeof(Vertex));
        renderer->SetIndexBuffer(indexBuffer);
        renderer->SetPrimitiveTopology(PrimitiveTopology::TriangleList);
        renderer->DrawIndexed(static_cast<uint32_t>(indices.size()));

        renderer->EndFrame();
        renderer->Present();
    }) << "Hello Triangle render cycle should not throw exceptions";

    // Clean up resources
    renderer->DestroyBuffer(vertexBuffer);
    renderer->DestroyBuffer(indexBuffer);
    renderer->DestroyShader(colorShader);
}

// The main function that runs all the tests
int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
