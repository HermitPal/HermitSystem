#pragma once

#include <cmath>
#include <limits>

namespace Math
{
/**
 * @brief A 2D vector class for mathematical operations
 *
 * Provides standard vector operations including arithmetic,
 * normalization, dot/cross products, and utility functions.
 */
class Vector2
{
  public:
    // Member variables
    float x, y;

    // Constructors
    Vector2()
        : x(0.0f), y(0.0f)
    {
    }
    Vector2(float x, float y)
        : x(x), y(y)
    {
    }
    Vector2(const Vector2& other)
        : x(other.x), y(other.y)
    {
    }

    // Assignment operator
    Vector2& operator=(const Vector2& other)
    {
        if (this != &other)
        {
            x = other.x;
            y = other.y;
        }
        return *this;
    }

    // Arithmetic operators
    Vector2 operator+(const Vector2& other) const
    {
        return Vector2(x + other.x, y + other.y);
    }

    Vector2 operator-(const Vector2& other) const
    {
        return Vector2(x - other.x, y - other.y);
    }

    Vector2 operator*(float scalar) const
    {
        return Vector2(x * scalar, y * scalar);
    }

    Vector2 operator/(float scalar) const
    {
        return Vector2(x / scalar, y / scalar);
    }

    Vector2 operator-() const
    {
        return Vector2(-x, -y);
    }

    // Compound assignment operators
    Vector2& operator+=(const Vector2& other)
    {
        x += other.x;
        y += other.y;
        return *this;
    }

    Vector2& operator-=(const Vector2& other)
    {
        x -= other.x;
        y -= other.y;
        return *this;
    }

    Vector2& operator*=(float scalar)
    {
        x *= scalar;
        y *= scalar;
        return *this;
    }

    Vector2& operator/=(float scalar)
    {
        x /= scalar;
        y /= scalar;
        return *this;
    }

    // Comparison operators
    bool operator==(const Vector2& other) const
    {
        return (x == other.x) && (y == other.y);
    }

    bool operator!=(const Vector2& other) const
    {
        return !(*this == other);
    }

    // Vector operations

    /**
     * @brief Calculate the magnitude (length) of the vector
     * @return The magnitude as a float
     */
    float Magnitude() const
    {
        return std::sqrt(x * x + y * y);
    }

    /**
     * @brief Calculate the squared magnitude of the vector
     * @return The squared magnitude as a float
     * @note More efficient than Magnitude() when you only need to compare lengths
     */
    float MagnitudeSquared() const
    {
        return x * x + y * y;
    }

    /**
     * @brief Normalize this vector in-place
     * @note If the vector has zero length, it remains unchanged
     */
    void Normalize()
    {
        float mag = Magnitude();
        if (mag > 1e-12f)
        {
            x /= mag;
            y /= mag;
        }
    }

    /**
     * @brief Return a normalized copy of this vector
     * @return A new Vector2 with unit length
     * @note If the vector has zero length, returns a zero vector
     */
    Vector2 Normalized() const
    {
        Vector2 result(*this);
        result.Normalize();
        return result;
    }

    // Static utility functions

    /**
     * @brief Calculate the dot product of two vectors
     * @param a First vector
     * @param b Second vector
     * @return The dot product as a float
     */
    static float Dot(const Vector2& a, const Vector2& b)
    {
        return a.x * b.x + a.y * b.y;
    }

    /**
     * @brief Calculate the cross product of two vectors
     * @param a First vector
     * @param b Second vector
     * @return The cross product as a float (z-component of the 3D cross product)
     */
    static float Cross(const Vector2& a, const Vector2& b)
    {
        return a.x * b.y - a.y * b.x;
    }

    /**
     * @brief Calculate the distance between two points
     * @param a First point
     * @param b Second point
     * @return The distance as a float
     */
    static float Distance(const Vector2& a, const Vector2& b)
    {
        return (b - a).Magnitude();
    }

    /**
     * @brief Calculate the squared distance between two points
     * @param a First point
     * @param b Second point
     * @return The squared distance as a float
     * @note More efficient than Distance() when you only need to compare distances
     */
    static float DistanceSquared(const Vector2& a, const Vector2& b)
    {
        return (b - a).MagnitudeSquared();
    }

    /**
     * @brief Linear interpolation between two vectors
     * @param a Start vector
     * @param b End vector
     * @param t Interpolation parameter (0.0 = a, 1.0 = b)
     * @return Interpolated vector
     */
    static Vector2 Lerp(const Vector2& a, const Vector2& b, float t)
    {
        return a + (b - a) * t;
    }

    /**
     * @brief Clamp interpolation parameter and perform linear interpolation
     * @param a Start vector
     * @param b End vector
     * @param t Interpolation parameter (clamped to [0,1])
     * @return Interpolated vector
     */
    static Vector2 LerpClamped(const Vector2& a, const Vector2& b, float t)
    {
        if (t < 0.0f)
            t = 0.0f;
        if (t > 1.0f)
            t = 1.0f;
        return Lerp(a, b, t);
    }

    /**
     * @brief Calculate the angle between two vectors in radians
     * @param a First vector
     * @param b Second vector
     * @return Angle in radians
     */
    static float Angle(const Vector2& a, const Vector2& b)
    {
        float dot = Dot(a.Normalized(), b.Normalized());
        // Clamp to handle floating point errors
        if (dot < -1.0f)
            dot = -1.0f;
        if (dot > 1.0f)
            dot = 1.0f;
        return std::acos(dot);
    }

    /**
     * @brief Project vector a onto vector b
     * @param a Vector to project
     * @param b Vector to project onto
     * @return Projected vector
     */
    static Vector2 Project(const Vector2& a, const Vector2& b)
    {
        float dot = Dot(a, b);
        float magSq = b.MagnitudeSquared();
        if (magSq > std::numeric_limits<float>::epsilon())
        {
            return b * (dot / magSq);
        }
        return Zero();
    }

    /**
     * @brief Reflect vector a across normal n
     * @param a Vector to reflect
     * @param n Normal vector (should be normalized)
     * @return Reflected vector
     */
    static Vector2 Reflect(const Vector2& a, const Vector2& n)
    {
        return a - n * (2.0f * Dot(a, n));
    }

    // Static constant vectors
    static Vector2 Zero()
    {
        return Vector2(0.0f, 0.0f);
    }
    static Vector2 One()
    {
        return Vector2(1.0f, 1.0f);
    }
    static Vector2 Up()
    {
        return Vector2(0.0f, 1.0f);
    }
    static Vector2 Down()
    {
        return Vector2(0.0f, -1.0f);
    }
    static Vector2 Left()
    {
        return Vector2(-1.0f, 0.0f);
    }
    static Vector2 Right()
    {
        return Vector2(1.0f, 0.0f);
    }
};

// Non-member operators for scalar multiplication (scalar * vector)
inline Vector2 operator*(float scalar, const Vector2& vector)
{
    return vector * scalar;
}

} // namespace Math