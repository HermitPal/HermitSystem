# `Math` Namespace Architecture

This document outlines the architecture of the `Math` namespace, a foundational library for handling 3D mathematics, specifically tailored for graphics programming.

## Core Philosophy

The `Math` namespace is designed to be a simple, efficient, and intuitive header-only library, heavily inspired by the popular [GLM (OpenGL Mathematics)](https://github.com/g-truc/glm) library. Its core philosophy is to provide C++ code with a syntax that closely mirrors the mathematics used in graphics shaders (GLSL), making it easy to translate algorithms between CPU and GPU code. It prioritizes:

-   **Simplicity:** A clean, easy-to-understand API.
-   **Header-Only:** For ease of integration. There are no `.cpp` files to compile; simply include the headers.
-   **Graphics API Compatibility:** Matrix memory layout is column-major by default, matching the standard expected by modern graphics APIs like OpenGL, Vulkan, and DirectX.

## Architecture Overview

The library is organized into a collection of header files, each focused on a specific component of 3D math. A single convenience header, `Math.h`, is provided to include the entire library.

### Dependency Structure

The library is self-contained, with more complex components building upon simpler ones. The application can include the main `Math.h` header to get access to all functionality.

```
+-----------------+
|   Application   |
+-----------------+
        |
        | #include "Math/Math.h"
        v
+--------------------------------------------------------------------+
|                               Math.h                               |
+--------------------------------------------------------------------+
|          |              |                 |               |
| Includes | Includes     | Includes        | Includes      | Includes
+----------+--------------+-----------------+---------------+----------+
     |          |              |                 |               |
     v          v              v                 v               v
+--------+ +--------+ +--------+ +-------------------+ +---------------+ 
| Vec3.h | | Vec4.h | | Mat4.h | | Transformations.h | | Projections.h |
+--------+ +--------+ +--------+ +-------------------+ +---------------+ 
```

## Key Components

### `Vec3.h`

-   **Purpose:** Defines a 3-component vector (`Vec3`) of floats.
-   **Contents:**
    -   `struct Vec3 { float x, y, z; }`
    -   Overloaded operators for basic vector arithmetic (`+`, `-`, `*`, `/`).
    -   Free functions for common operations: `dot()`, `cross()`, `length()`, `normalize()`.

### `Vec4.h`

-   **Purpose:** Defines a 4-component vector (`Vec4`) of floats.
-   **Contents:**
    -   `struct Vec4 { float x, y, z, w; }`
    -   Overloaded operators for basic vector arithmetic.

### `Mat4.h`

-   **Purpose:** Defines a 4x4 matrix (`Mat4`) for representing transformations in 3D space.
-   **Contents:**
    -   `struct Mat4`: An array of four `Vec4`s, representing the columns.
    -   **Column-Major Layout:** This is a critical design choice. The matrix is stored in memory one column at a time. This allows the matrix data to be sent directly to the GPU without needing to be transposed first.
    -   The default constructor initializes the matrix to the identity matrix.
    -   Overloaded `*` operator for matrix-matrix multiplication.

### `Transformations.h`

-   **Purpose:** Provides functions to create the three fundamental 3D transformation matrices.
-   **Contents:**
    -   `translate(Mat4, Vec3)`: Builds a translation matrix.
    -   `rotate(Mat4, angle, axis)`: Builds a rotation matrix around a given axis.
    -   `scale(Mat4, Vec3)`: Builds a scale matrix.

### `Projections.h`

-   **Purpose:** Provides functions to create camera projection matrices.
-   **Contents:**
    -   `perspective(fov, aspect, near, far)`: Creates a perspective projection matrix, which gives the scene a sense of depth.

## How to Use

The primary way to use the library is to include the main `Math.h` header. From there, you can construct transformation matrices and perform calculations.

**Example: Creating a Model-View-Projection (MVP) Matrix**

```cpp
#include "Math/Math.h"
#include <iostream> // For printing

// ... in your application code ...

// Use the namespace for convenience
using namespace Math;

// 1. Create a model matrix to position an object in the world
// Start with an identity matrix
Mat4 modelMatrix;
// Translate the object
modelMatrix = translate(modelMatrix, {5.0f, 0.0f, 0.0f});
// Rotate it 90 degrees around the Y axis
modelMatrix = rotate(modelMatrix, 1.5708f, {0.0f, 1.0f, 0.0f});
// Scale it to be half its size
modelMatrix = scale(modelMatrix, {0.5f, 0.5f, 0.5f});

// 2. Create a view matrix (camera)
// This would typically be managed by a camera class.
// For this example, we'll create a simple one looking at the origin from a distance.
Mat4 viewMatrix = translate({}, {0.0f, 0.0f, -10.0f}); // Note: simplified view matrix

// 3. Create a perspective projection matrix
float fov = 1.5708f; // 90 degrees in radians
float aspectRatio = 16.0f / 9.0f;
Mat4 projectionMatrix = perspective(fov, aspectRatio, 0.1f, 100.0f);

// 4. Combine them into the final MVP matrix
// The order (P * V * M) is important.
Mat4 mvpMatrix = projectionMatrix * viewMatrix * modelMatrix;

// This mvpMatrix is now ready to be sent to a shader.
```
