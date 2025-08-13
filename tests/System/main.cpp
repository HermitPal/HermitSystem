#include "System/IInput.h"
#include "System/IWindow.h"
#include "System/SystemFactory.h"
#include <gtest/gtest.h>
#include <memory>

using namespace System;

// Test fixture for System tests
class SystemTest : public ::testing::Test
{
  protected:
    void SetUp() override
    {
        // Set up a basic window configuration for tests
        config.title = "Test Window";
        config.width = 800;
        config.height = 600;
        config.resizable = true;
        config.vsync = false; // Disable for testing
        config.posX = 100;
        config.posY = 100;
    }

    void TearDown() override
    {
        // Clean up any resources
        if (window)
        {
            window->Shutdown();
            window.reset();
        }
    }

    WindowConfig config;
    std::unique_ptr<IWindow> window;
};

// Basic test to ensure the framework is set up correctly
TEST(SetupTest, HelloWorld)
{
    ASSERT_EQ(7, 7);
}

// Test SystemFactory functionality
TEST_F(SystemTest, FactoryCreatesWindow)
{
    window = SystemFactory::CreateApplicationWindow(config);

    ASSERT_NE(window, nullptr) << "Factory should create a valid window";
    EXPECT_NE(window->GetInput(), nullptr) << "Window should have an associated input system";
}

TEST_F(SystemTest, FactoryCreatesWindowWithCorrectConfig)
{
    window = SystemFactory::CreateApplicationWindow(config);

    ASSERT_NE(window, nullptr);
    // Don't call Initialize() - the factory should have done it already

    // Test that the window was created with the correct configuration
    EXPECT_EQ(window->GetTitle(), config.title);

    int width, height;
    window->GetSize(width, height);
    EXPECT_EQ(width, config.width);
    EXPECT_EQ(height, config.height);
}

// Test Window functionality
TEST_F(SystemTest, WindowInitialization)
{
    window = SystemFactory::CreateApplicationWindow(config);

    ASSERT_NE(window, nullptr);

    // Window should already be initialized by the factory
    // Trying to initialize again should return false (already initialized)
    EXPECT_FALSE(window->Initialize(config)) << "Window should already be initialized by factory";

    // Window should not be closed initially
    EXPECT_FALSE(window->ShouldClose()) << "New window should not be marked for closing";
}

TEST_F(SystemTest, WindowProperties)
{
    window = SystemFactory::CreateApplicationWindow(config);

    ASSERT_NE(window, nullptr);
    // Factory should have already initialized the window

    // Test property getters
    EXPECT_EQ(window->GetTitle(), "Test Window");

    int width, height;
    window->GetSize(width, height);
    EXPECT_EQ(width, 800);
    EXPECT_EQ(height, 600);

    EXPECT_FALSE(window->IsVSyncEnabled()); // We set vsync to false in config

    // Test property setters
    window->SetTitle("New Title");
    EXPECT_EQ(window->GetTitle(), "New Title");

    window->SetSize(1024, 768);
    window->GetSize(width, height);
    EXPECT_EQ(width, 1024);
    EXPECT_EQ(height, 768);

    window->SetPosition(200, 150);
    int x, y;
    window->GetPosition(x, y);
    EXPECT_EQ(x, 200);
    EXPECT_EQ(y, 150);
}

TEST_F(SystemTest, WindowVisibility)
{
    window = SystemFactory::CreateApplicationWindow(config);

    ASSERT_NE(window, nullptr);
    // Window should already be initialized by factory

    // Window should be hidden by default
    EXPECT_FALSE(window->IsVisible()) << "Window should be hidden by default";

    // Show window
    window->Show();
    EXPECT_TRUE(window->IsVisible()) << "Window should be visible after Show()";

    // Hide window
    window->Hide();
    EXPECT_FALSE(window->IsVisible()) << "Window should be hidden after Hide()";
}

TEST_F(SystemTest, WindowNativeHandle)
{
    window = SystemFactory::CreateApplicationWindow(config);

    ASSERT_NE(window, nullptr);
    // Window should already be initialized by factory

    // Native handle should be valid after initialization
    EXPECT_NE(window->GetNativeHandle(), nullptr) << "Native handle should be valid after initialization";
}

// Test Input System functionality
TEST_F(SystemTest, InputSystemExists)
{
    window = SystemFactory::CreateApplicationWindow(config);

    ASSERT_NE(window, nullptr);

    auto input = window->GetInput();
    ASSERT_NE(input, nullptr) << "Window should provide a valid input system";
}

TEST_F(SystemTest, InputPollingInterface)
{
    window = SystemFactory::CreateApplicationWindow(config);
    ASSERT_NE(window, nullptr);
    // Window should already be initialized by factory

    auto input = window->GetInput();
    ASSERT_NE(input, nullptr);

    // Test key polling methods exist and return valid values
    EXPECT_NO_THROW(input->IsKeyDown(Key::A)) << "IsKeyDown should not throw";
    EXPECT_NO_THROW(input->WasKeyPressed(Key::Space)) << "WasKeyPressed should not throw";
    EXPECT_NO_THROW(input->WasKeyReleased(Key::Escape)) << "WasKeyReleased should not throw";

    // Test mouse polling methods
    int x, y;
    EXPECT_NO_THROW(input->GetMousePosition(x, y)) << "GetMousePosition should not throw";
    EXPECT_NO_THROW(input->IsMouseButtonDown(MouseButton::Left)) << "IsMouseButtonDown should not throw";
}

TEST_F(SystemTest, InputCallbackInterface)
{
    window = SystemFactory::CreateApplicationWindow(config);
    ASSERT_NE(window, nullptr);
    // Window should already be initialized by factory

    auto input = window->GetInput();
    ASSERT_NE(input, nullptr);

    // Test that callback setters don't crash
    bool keyCallbackCalled = false;
    bool mouseCallbackCalled = false;

    EXPECT_NO_THROW(input->SetKeyCallback([&](Key key, bool pressed) {
        keyCallbackCalled = true;
    })) << "SetKeyCallback should not throw";

    EXPECT_NO_THROW(input->SetMouseButtonCallback([&](MouseButton button, bool pressed, int x, int y) {
        mouseCallbackCalled = true;
    })) << "SetMouseButtonCallback should not throw";

    EXPECT_NO_THROW(input->SetMouseMoveCallback([](int x, int y) {
        // Mouse move callback
    })) << "SetMouseMoveCallback should not throw";
}

// Test Window Callbacks
TEST_F(SystemTest, WindowCallbacks)
{
    window = SystemFactory::CreateApplicationWindow(config);
    ASSERT_NE(window, nullptr);
    // Window should already be initialized by factory

    bool resizeCallbackCalled = false;
    bool closeCallbackCalled = false;
    bool focusCallbackCalled = false;

    // Set up callbacks
    EXPECT_NO_THROW(window->SetResizeCallback([&](int width, int height) {
        resizeCallbackCalled = true;
        EXPECT_GT(width, 0);
        EXPECT_GT(height, 0);
    })) << "SetResizeCallback should not throw";

    EXPECT_NO_THROW(window->SetCloseCallback([&]() {
        closeCallbackCalled = true;
    })) << "SetCloseCallback should not throw";

    EXPECT_NO_THROW(window->SetFocusCallback([&](bool hasFocus) {
        focusCallbackCalled = true;
    })) << "SetFocusCallback should not throw";
}

// Test error conditions
TEST_F(SystemTest, InvalidConfigurations)
{
    // Test with invalid dimensions
    WindowConfig invalidConfig = config;
    invalidConfig.width = 0;
    invalidConfig.height = 0;

    window = SystemFactory::CreateApplicationWindow(invalidConfig);
    // The factory might still create a window but with corrected dimensions,
    // or it might return nullptr depending on implementation
    // This test documents the expected behavior
    if (window)
    {
        int width, height;
        window->GetSize(width, height);
        EXPECT_GT(width, 0) << "Window width should be corrected to positive value";
        EXPECT_GT(height, 0) << "Window height should be corrected to positive value";
    }
}

TEST_F(SystemTest, WindowLifecycle)
{
    window = SystemFactory::CreateApplicationWindow(config);
    ASSERT_NE(window, nullptr);

    // Factory should have already initialized the window
    // Test that it's properly initialized
    EXPECT_FALSE(window->ShouldClose()) << "Window should not be marked for closing initially";

    window->Show();
    EXPECT_TRUE(window->IsVisible()) << "Window should be visible";

    // Simulate a frame update
    EXPECT_NO_THROW(window->Update()) << "Window update should not throw";

    // Cleanup
    EXPECT_NO_THROW(window->Shutdown()) << "Window shutdown should not throw";
}

// Test Key enum values
TEST(InputEnumTest, KeyEnumValues)
{
    // Test that key enum has expected values
    EXPECT_NE(static_cast<int>(Key::A), static_cast<int>(Key::B)) << "Different keys should have different values";
    EXPECT_NE(static_cast<int>(Key::Space), static_cast<int>(Key::Enter)) << "Special keys should have unique values";
    EXPECT_NE(static_cast<int>(Key::Escape), static_cast<int>(Key::Tab)) << "Control keys should have unique values";
}

// Test MouseButton enum values
TEST(InputEnumTest, MouseButtonEnumValues)
{
    // Test that mouse button enum has expected values
    EXPECT_NE(static_cast<int>(MouseButton::Left), static_cast<int>(MouseButton::Right)) << "Mouse buttons should have different values";
    EXPECT_NE(static_cast<int>(MouseButton::Right), static_cast<int>(MouseButton::Middle)) << "Mouse buttons should have different values";
}

// Integration test that mimics main.cpp usage
TEST_F(SystemTest, MainApplicationFlow)
{
    // This test simulates the flow in main.cpp

    // Step 1: Create window
    window = SystemFactory::CreateApplicationWindow(config);
    ASSERT_NE(window, nullptr);

    // Step 2: Get input system
    auto input = window->GetInput();
    ASSERT_NE(input, nullptr);

    // Step 3: Factory should have already initialized the window
    // Verify it's properly initialized
    EXPECT_NE(window->GetNativeHandle(), nullptr) << "Native handle should be valid";

    // Step 4: Set up callbacks (should not throw)
    EXPECT_NO_THROW(window->SetResizeCallback([](int w, int h) { /* resize handler */ }));
    EXPECT_NO_THROW(window->SetCloseCallback([]() { /* close handler */ }));
    EXPECT_NO_THROW(input->SetKeyCallback([](Key key, bool pressed) { /* key handler */ }));
    EXPECT_NO_THROW(input->SetMouseButtonCallback([](MouseButton btn, bool pressed, int x, int y) { /* mouse handler */ }));

    // Step 5: Show window
    EXPECT_NO_THROW(window->Show());
    EXPECT_TRUE(window->IsVisible());

    // Step 6: Simulate a few update cycles
    for (int i = 0; i < 5; ++i)
    {
        EXPECT_NO_THROW(window->Update()) << "Window update cycle " << i << " should not throw";
        EXPECT_FALSE(window->ShouldClose()) << "Window should not be marked for closing during normal operation";

        // Test input polling
        EXPECT_NO_THROW(input->IsKeyDown(Key::W));
        EXPECT_NO_THROW(input->WasKeyPressed(Key::Space));

        int mouseX, mouseY;
        EXPECT_NO_THROW(input->GetMousePosition(mouseX, mouseY));
    }

    // Step 7: Cleanup
    EXPECT_NO_THROW(window->Shutdown());
}

// The main function that runs all the tests
int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}