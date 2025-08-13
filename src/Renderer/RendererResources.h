#pragma once

#include "../Math/Vector3.h"
#include <cstdint>
#include <vector>

namespace Renderer
{

// Basic Vertex structure
struct Vertex
{
    Math::Vector3 position;
    // Add color for now, can expand with UVs, normals later
    struct
    {
        float r, g, b, a;
    } color;
};

// Primitive Topology
enum class PrimitiveTopology
{
    TriangleList,
    LineList,
    PointList
};

// Buffer Usage
enum class BufferUsage
{
    Default, // GPU read/write
    Immutable, // GPU read only, initialized once
    Dynamic, // CPU write, GPU read
    Staging // CPU read/write, used for data transfer
};

// Buffer Type
enum class BufferType
{
    VertexBuffer,
    IndexBuffer,
    ConstantBuffer
};

// Forward declarations for resource handles
// These will be opaque pointers or IDs managed by the concrete renderer
using BufferHandle = void*;
using ShaderHandle = void*;

// Basic Mesh structure (for CPU-side data)
struct Mesh
{
    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;
};

} // namespace Renderer
