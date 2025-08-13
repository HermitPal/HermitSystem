# `Systsem` Namespace Architecture

This document outlines the architecture of the `Systsem` namespace, a library designed to provide a platform-agnostic interface for creating and managing application windows and handling user input.

## Core Philosophy

The primary goal of the `Systsem` namespace is to decouple the main application logic from the underlying operating system's specifics. This is achieved through a combination of interface-based design and the factory pattern, allowing developers to write their application code once and have it run on multiple platforms (e.g., Windows, Linux, macOS) by simply linking against the correct platform-specific implementation.

## Architecture Overview

The architecture is composed of three main parts:

1.  **Abstract Interfaces (`IWindow`, `IInput`):** These define the public-facing API of the library. They provide a contract for what a "Window" and an "Input System" can do, without exposing any implementation details.
2.  **Platform-Specific Implementations (`Win32Window`, `Win32Input`):** These are the concrete classes that implement the interfaces for a specific operating system. Currently, only a Win32 (Windows) implementation is provided.
3.  **Factory (`SystemFactory`):** This is the entry point for the client application. It is responsible for creating a matched pair of window and input system objects for the platform the application is being compiled on.

### Architectural Diagram

```
+-----------------+
|   Application   |
|    (main.cpp)   |
+-----------------+
        |
        | Asks for a window
        v
+-----------------+
| SystemFactory   | Creates a matched pair of...
+-----------------+--------------------------------------+
        |                                              |
        | Creates                                      | Creates
        v                                              v
+-----------------+                            +-----------------+
|  IWindow        | (Interface)                |  IInput         | (Interface)
+-----------------+                            +-----------------+
        ^                                              ^
        | Implements                                   | Implements
+-----------------+
|  Win32Window    | (Concrete Impl)            +-----------------+
+-----------------+----owns-a--->-------------|  Win32Input     | (Concrete Impl)
                                               +-----------------+
```

## Key Components

### `IWindow`

-   **Purpose:** An abstract interface for all window-related functionality.
-   **Responsibilities:** Window lifecycle (creation, update loop, shutdown), properties (title, size, position), and state (fullscreen, focus, visibility). It also provides access to its associated `IInput` system.
-   **Key Methods:** `Initialize()`, `Update()`, `Shutdown()`, `GetInput()`.

### `IInput`

-   **Purpose:** An abstract interface for all user input.
-   **Responsibilities:** Provides methods for both polling-based input (`IsKeyDown`, `GetMousePosition`) and event-based input (callbacks like `SetKeyCallback`). It defines platform-agnostic enums for keys and mouse buttons.
-   **Key Methods:** `IsKeyDown()`, `WasKeyPressed()`, `GetMousePosition()`, `SetKeyCallback()`.

### `SystemFactory`

-   **Purpose:** To abstract and centralize the creation of platform-specific objects.
-   **Responsibilities:** Contains a single static method, `CreateApplicationWindow()`, which determines the current platform at compile time and instantiates the correct concrete `IWindow` and `IInput` pair.
-   **Benefit:** This is the crucial component that guarantees a `Win32Window` is always paired with a `Win32Input`, preventing runtime errors and simplifying the client code in `main.cpp`.

### `Win32Window` & `Win32Input`

-   **Purpose:** The concrete implementations of `IWindow` and `IInput` for the Microsoft Windows (Win32) API.
-   **Responsibilities:** They handle the low-level details of creating a window, running the message loop, and translating Windows messages (like `WM_KEYDOWN`) into the platform-agnostic events and states defined by the interfaces.

## How to Use

The client application (in `main.cpp`) interacts with the `Systsem` library as follows:

1.  Includes the necessary headers: `SystemFactory.h`, `IWindow.h`, `IInput.h`.
2.  Creates a `WindowConfig` struct to define the desired window properties.
3.  Calls `Systsem::SystemFactory::CreateApplicationWindow(config)` to get a `std::unique_ptr<IWindow>`.
4.  Retrieves the associated input system via `window->GetInput()`.
5.  Programs the rest of the application using only the `IWindow` and `IInput` interfaces.

This design ensures that the main application logic remains clean, portable, and completely unaware of the underlying platform it is running on.
