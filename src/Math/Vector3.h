#pragma once

namespace Math
{
/**
 * @brief A 3D vector class for mathematical operations
 *
 * Provides standard vector operations including arithmetic,
 * normalization, dot/cross products, and utility functions.
 */
class Vector3
{
  public:
    // Member variables
    float x, y, z;

    // Constructors
    Vector3();
    Vector3(float x, float y, float z);
    Vector3(const Vector3& other);

    // Assignment operator
    Vector3& operator=(const Vector3& other);

    // Arithmetic operators
    Vector3 operator+(const Vector3& other) const;
    Vector3 operator-(const Vector3& other) const;
    Vector3 operator*(float scalar) const;
    Vector3 operator/(float scalar) const;
    Vector3 operator-() const;

    // Compound assignment operators
    Vector3& operator+=(const Vector3& other);
    Vector3& operator-=(const Vector3& other);
    Vector3& operator*=(float scalar);
    Vector3& operator/=(float scalar);

    // Comparison operators
    bool operator==(const Vector3& other) const;
    bool operator!=(const Vector3& other) const;

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
     * @return A new Vector3 with unit length
     * @note If the vector has zero length, returns a zero vector
     */
    Vector3 Normalized() const;

    // Static utility functions

    /**
     * @brief Calculate the dot product of two vectors
     * @param a First vector
     * @param b Second vector
     * @return The dot product as a float
     */
    static float Dot(const Vector3& a, const Vector3& b);

    /**
     * @brief Calculate the cross product of two vectors
     * @param a First vector
     * @param b Second vector
     * @return The cross product as a Vector3
     */
    static Vector3 Cross(const Vector3& a, const Vector3& b);

    /**
     * @brief Calculate the distance between two points
     * @param a First point
     * @param b Second point
     * @return The distance as a float
     */
    static float Distance(const Vector3& a, const Vector3& b);

    /**
     * @brief Calculate the squared distance between two points
     * @param a First point
     * @param b Second point
     * @return The squared distance as a float
     * @note More efficient than Distance() when you only need to compare distances
     */
    static float DistanceSquared(const Vector3& a, const Vector3& b);

    /**
     * @brief Linear interpolation between two vectors
     * @param a Start vector
     * @param b End vector
     * @param t Interpolation parameter (0.0 = a, 1.0 = b)
     * @return Interpolated vector
     */
    static Vector3 Lerp(const Vector3& a, const Vector3& b, float t);

    /**
     * @brief Clamp interpolation parameter and perform linear interpolation
     * @param a Start vector
     * @param b End vector
     * @param t Interpolation parameter (clamped to [0,1])
     * @return Interpolated vector
     */
    static Vector3 LerpClamped(const Vector3& a, const Vector3& b, float t);

    /**
     * @brief Calculate the angle between two vectors in radians
     * @param a First vector
     * @param b Second vector
     * @return Angle in radians
     */
    static float Angle(const Vector3& a, const Vector3& b);

    /**
     * @brief Project vector a onto vector b
     * @param a Vector to project
     * @param b Vector to project onto
     * @return Projected vector
     */
    static Vector3 Project(const Vector3& a, const Vector3& b);

    /**
     * @brief Reflect vector a across normal n
     * @param a Vector to reflect
     * @param n Normal vector (should be normalized)
     * @return Reflected vector
     */
    static Vector3 Reflect(const Vector3& a, const Vector3& n);

    /**
     * @brief Calculate the scalar triple product of three vectors
     * @param a First vector
     * @param b Second vector
     * @param c Third vector
     * @return Scalar triple product (a · (b × c))
     * @note Represents the signed volume of the parallelepiped formed by the three vectors
     */
    static float ScalarTripleProduct(const Vector3& a, const Vector3& b, const Vector3& c);

    /**
     * @brief Calculate the vector triple product of three vectors
     * @param a First vector
     * @param b Second vector
     * @param c Third vector
     * @return Vector triple product (a × (b × c))
     */
    static Vector3 VectorTripleProduct(const Vector3& a, const Vector3& b, const Vector3& c);

    /**
     * @brief Spherical linear interpolation between two vectors
     * @param a Start vector
     * @param b End vector
     * @param t Interpolation parameter (0.0 = a, 1.0 = b)
     * @return Spherically interpolated vector
     * @note Both vectors should be normalized for best results
     */
    static Vector3 Slerp(const Vector3& a, const Vector3& b, float t);

    /**
     * @brief Create a vector that is orthogonal to the given vector
     * @param v Input vector
     * @return A vector perpendicular to the input
     * @note The returned vector is not normalized
     */
    static Vector3 Orthogonal(const Vector3& v);

    /**
     * @brief Rotate a vector around an axis by the given angle
     * @param v Vector to rotate
     * @param axis Axis of rotation (should be normalized)
     * @param angle Angle in radians
     * @return Rotated vector
     */
    static Vector3 RotateAroundAxis(const Vector3& v, const Vector3& axis, float angle);

    // Static constant vectors
    static Vector3 Zero();
    static Vector3 One();
    static Vector3 Up();
    static Vector3 Down();
    static Vector3 Left();
    static Vector3 Right();
    static Vector3 Forward();
    static Vector3 Back();
};

// Non-member operators for scalar multiplication (scalar * vector)
Vector3 operator*(float scalar, const Vector3& vector);

} // namespace Math