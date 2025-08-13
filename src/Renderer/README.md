# `Renderer` Namespace Architecture

This document outlines the architecture of the `Renderer` namespace, a library designed to provide a platform-agnostic interface for rendering graphics.

## Core Philosophy

The primary goal of the `Renderer` namespace is to abstract the underlying graphics API (e.g., DirectX 12, Vulkan) from the main application logic. This is achieved through an interface-based design and a factory pattern, allowing developers to write their rendering code once and have it run on different graphics APIs by simply choosing a different renderer at runtime.

## Architecture Overview

The architecture is composed of three main parts:

1.  **Abstract Interface (`IRenderer`):** This defines the public-facing API of the library. It provides a contract for what a "Renderer" can do, without exposing any implementation details of a specific graphics API.
2.  **Platform-Specific Implementations (`DirectX12Renderer`):** These are the concrete classes that implement the `IRenderer` interface for a specific graphics API. Currently, only a DirectX 12 implementation is provided.
3.  **Factory (`RendererFactory`):** This is the entry point for the client application. It is responsible for creating an instance of a renderer based on the requested or best-available graphics API.

### Architectural Diagram

```
+-----------------+
|   Application   |
|    (main.cpp)   |
+-----------------+
        |
        | Asks for a renderer
        v
+-----------------+
| RendererFactory | Creates a renderer for a specific API
+-----------------+--------------------------------------+
        |
        | Creates
        v
+-----------------+
|  IRenderer      | (Interface)
+-----------------+
        ^
        | Implements
+-----------------+
| DirectX12Renderer| (Concrete Impl)
+-----------------+
```

## Key Components

### `IRenderer`

-   **Purpose:** An abstract interface for all rendering functionality.
-   **Responsibilities:** Manages the rendering lifecycle (initialization, shutdown), frame management (begin, end, present), and basic rendering operations (clear, set viewport).
-   **Key Methods:** `Initialize()`, `Shutdown()`, `BeginFrame()`, `EndFrame()`, `Present()`, `Clear()`.

### `RendererFactory`

-   **Purpose:** To abstract and centralize the creation of platform-specific renderer objects.
-   **Responsibilities:** Contains static methods to create a renderer for a specific API (`CreateRenderer(RendererAPI)`), or to create the best available renderer for the current platform (`CreateRenderer()`). It can also be used to check for API support.
-   **Benefit:** This component simplifies the client code in `main.cpp` by handling the details of renderer creation.

### `DirectX12Renderer`

-   **Purpose:** The concrete implementation of `IRenderer` for the Microsoft DirectX 12 API.
-   **Responsibilities:** It handles the low-level details of creating a DirectX 12 device, managing swap chains, command lists, and other DirectX 12-specific objects.

## How to Use

The client application (in `main.cpp`) interacts with the `Renderer` library as follows:

1.  Includes the necessary headers: `RendererFactory.h`, `IRenderer.h`.
2.  Calls `Renderer::RendererFactory::CreateRenderer()` to get a `std::unique_ptr<IRenderer>`. The factory can be used to select a specific renderer API if desired.
3.  Initializes the renderer with a window handle.
4.  Programs the rendering logic using only the `IRenderer` interface.

This design ensures that the main application logic remains clean, portable, and completely unaware of the underlying graphics API it is running on.
