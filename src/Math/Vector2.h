#pragma once

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
    Vector2();
    Vector2(float x, float y);
    Vector2(const Vector2& other);

    // Assignment operator
    Vector2& operator=(const Vector2& other);

    // Arithmetic operators
    Vector2 operator+(const Vector2& other) const;
    Vector2 operator-(const Vector2& other) const;
    Vector2 operator*(float scalar) const;
    Vector2 operator/(float scalar) const;
    Vector2 operator-() const;

    // Compound assignment operators
    Vector2& operator+=(const Vector2& other);
    Vector2& operator-=(const Vector2& other);
    Vector2& operator*=(float scalar);
    Vector2& operator/=(float scalar);

    // Comparison operators
    bool operator==(const Vector2& other) const;
    bool operator!=(const Vector2& other) const;

    // Vector operations

    /**
     * @brief Calculate the magnitude (length) of the vector
     * @return The magnitude as a float
     */
    float Magnitude() const;

    /**
     * @brief Calculate the squared magnitude of the vector
     * @return The squared magnitude as a float
     * @note More efficient than Magnitude() when you only need to compare lengths
     */
    float MagnitudeSquared() const;

    /**
     * @brief Normalize this vector in-place
     * @note If the vector has zero length, it remains unchanged
     */
    void Normalize();

    /**
     * @brief Return a normalized copy of this vector
     * @return A new Vector2 with unit length
     * @note If the vector has zero length, returns a zero vector
     */
    Vector2 Normalized() const;

    // Static utility functions

    /**
     * @brief Calculate the dot product of two vectors
     * @param a First vector
     * @param b Second vector
     * @return The dot product as a float
     */
    static float Dot(const Vector2& a, const Vector2& b);

    /**
     * @brief Calculate the cross product of two vectors
     * @param a First vector
     * @param b Second vector
     * @return The cross product as a float (z-component of the 3D cross product)
     */
    static float Cross(const Vector2& a, const Vector2& b);

    /**
     * @brief Calculate the distance between two points
     * @param a First point
     * @param b Second point
     * @return The distance as a float
     */
    static float Distance(const Vector2& a, const Vector2& b);

    /**
     * @brief Calculate the squared distance between two points
     * @param a First point
     * @param b Second point
     * @return The squared distance as a float
     * @note More efficient than Distance() when you only need to compare distances
     */
    static float DistanceSquared(const Vector2& a, const Vector2& b);

    /**
     * @brief Linear interpolation between two vectors
     * @param a Start vector
     * @param b End vector
     * @param t Interpolation parameter (0.0 = a, 1.0 = b)
     * @return Interpolated vector
     */
    static Vector2 Lerp(const Vector2& a, const Vector2& b, float t);

    /**
     * @brief Clamp interpolation parameter and perform linear interpolation
     * @param a Start vector
     * @param b End vector
     * @param t Interpolation parameter (clamped to [0,1])
     * @return Interpolated vector
     */
    static Vector2 LerpClamped(const Vector2& a, const Vector2& b, float t);

    /**
     * @brief Calculate the angle between two vectors in radians
     * @param a First vector
     * @param b Second vector
     * @return Angle in radians
     */
    static float Angle(const Vector2& a, const Vector2& b);

    /**
     * @brief Project vector a onto vector b
     * @param a Vector to project
     * @param b Vector to project onto
     * @return Projected vector
     */
    static Vector2 Project(const Vector2& a, const Vector2& b);

    /**
     * @brief Reflect vector a across normal n
     * @param a Vector to reflect
     * @param n Normal vector (should be normalized)
     * @return Reflected vector
     */
    static Vector2 Reflect(const Vector2& a, const Vector2& n);

    // Static constant vectors
    static Vector2 Zero();
    static Vector2 One();
    static Vector2 Up();
    static Vector2 Down();
    static Vector2 Left();
    static Vector2 Right();
};

// Non-member operators for scalar multiplication (scalar * vector)
Vector2 operator*(float scalar, const Vector2& vector);

} // namespace Math