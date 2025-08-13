#pragma once
#include <cmath>

namespace Math
{
    struct Vec3
    {
        float x = 0.0f, y = 0.0f, z = 0.0f;

        // Overloaded operators
        Vec3 operator+(const Vec3& other) const { return {x + other.x, y + other.y, z + other.z}; }
        Vec3 operator-(const Vec3& other) const { return {x - other.x, y - other.y, z - other.z}; }
        Vec3 operator*(float scalar) const { return {x * scalar, y * scalar, z * scalar}; }
        Vec3 operator/(float scalar) const { return {x / scalar, y / scalar, z / scalar}; }
    };

    // Common vector functions
    inline float dot(const Vec3& a, const Vec3& b)
    {
        return a.x * b.x + a.y * b.y + a.z * b.z;
    }

    inline float length(const Vec3& v)
    {
        return std::sqrt(dot(v, v));
    }

    inline Vec3 normalize(const Vec3& v)
    {
        return v / length(v);
    }

    inline Vec3 cross(const Vec3& a, const Vec3& b)
    {
        return {
            a.y * b.z - a.z * b.y,
            a.z * b.x - a.x * b.z,
            a.x * b.y - a.y * b.x
        };
    }
}
