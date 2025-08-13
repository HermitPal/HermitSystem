#pragma once

namespace Math
{
    struct Vec4
    {
        float x = 0.0f, y = 0.0f, z = 0.0f, w = 0.0f;

        // Overloaded operators
        Vec4 operator+(const Vec4& other) const { return {x + other.x, y + other.y, z + other.z, w + other.w}; }
        Vec4 operator-(const Vec4& other) const { return {x - other.x, y - other.y, z - other.z, w - other.w}; }
        Vec4 operator*(float scalar) const { return {x * scalar, y * scalar, z * scalar, w * scalar}; }
    };
}
