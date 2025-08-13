#include "Math/Vector3.h"
#include <algorithm>
#include <cmath>

namespace Math
{

// Constructors
Vector3::Vector3()
    : x(0.0f), y(0.0f), z(0.0f)
{
}

Vector3::Vector3(float x, float y, float z)
    : x(x), y(y), z(z)
{
}

Vector3::Vector3(const Vector3& other)
    : x(other.x), y(other.y), z(other.z)
{
}

// Assignment operator
Vector3& Vector3::operator=(const Vector3& other)
{
    if (this != &other)
    {
        x = other.x;
        y = other.y;
        z = other.z;
    }
    return *this;
}

// Arithmetic operators
Vector3 Vector3::operator+(const Vector3& other) const
{
    return Vector3(x + other.x, y + other.y, z + other.z);
}

Vector3 Vector3::operator-(const Vector3& other) const
{
    return Vector3(x - other.x, y - other.y, z - other.z);
}

Vector3 Vector3::operator*(float scalar) const
{
    return Vector3(x * scalar, y * scalar, z * scalar);
}

Vector3 Vector3::operator/(float scalar) const
{
    // Note: Division by zero check could be added here
    return Vector3(x / scalar, y / scalar, z / scalar);
}

Vector3 Vector3::operator-() const
{
    return Vector3(-x, -y, -z);
}

// Compound assignment operators
Vector3& Vector3::operator+=(const Vector3& other)
{
    x += other.x;
    y += other.y;
    z += other.z;
    return *this;
}

Vector3& Vector3::operator-=(const Vector3& other)
{
    x -= other.x;
    y -= other.y;
    z -= other.z;
    return *this;
}

Vector3& Vector3::operator*=(float scalar)
{
    x *= scalar;
    y *= scalar;
    z *= scalar;
    return *this;
}

Vector3& Vector3::operator/=(float scalar)
{
    // Note: Division by zero check could be added here
    x /= scalar;
    y /= scalar;
    z /= scalar;
    return *this;
}

// Comparison operators
bool Vector3::operator==(const Vector3& other) const
{
    const float epsilon = std::numeric_limits<float>::epsilon();
    return std::abs(x - other.x) < epsilon &&
           std::abs(y - other.y) < epsilon &&
           std::abs(z - other.z) < epsilon;
}

bool Vector3::operator!=(const Vector3& other) const
{
    return !(*this == other);
}

// Vector operations
float Vector3::Magnitude() const
{
    return std::sqrt(x * x + y * y + z * z);
}

float Vector3::MagnitudeSquared() const
{
    return x * x + y * y + z * z;
}

void Vector3::Normalize()
{
    float magnitude = Magnitude();
    if (magnitude > std::numeric_limits<float>::epsilon())
    {
        x /= magnitude;
        y /= magnitude;
        z /= magnitude;
    }
}

Vector3 Vector3::Normalized() const
{
    Vector3 result(*this);
    result.Normalize();
    return result;
}

// Static utility functions
float Vector3::Dot(const Vector3& a, const Vector3& b)
{
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

Vector3 Vector3::Cross(const Vector3& a, const Vector3& b)
{
    return Vector3(
        a.y * b.z - a.z * b.y,
        a.z * b.x - a.x * b.z,
        a.x * b.y - a.y * b.x);
}

float Vector3::Distance(const Vector3& a, const Vector3& b)
{
    return (b - a).Magnitude();
}

float Vector3::DistanceSquared(const Vector3& a, const Vector3& b)
{
    return (b - a).MagnitudeSquared();
}

Vector3 Vector3::Lerp(const Vector3& a, const Vector3& b, float t)
{
    return a + (b - a) * t;
}

Vector3 Vector3::LerpClamped(const Vector3& a, const Vector3& b, float t)
{
    t = std::max(0.0f, std::min(1.0f, t));
    return Lerp(a, b, t);
}

float Vector3::Angle(const Vector3& a, const Vector3& b)
{
    float dotProduct = Dot(a, b);
    float magnitudes = a.Magnitude() * b.Magnitude();

    if (magnitudes < std::numeric_limits<float>::epsilon())
        return 0.0f;

    // Clamp to avoid numerical errors in acos
    float cosAngle = std::max(-1.0f, std::min(1.0f, dotProduct / magnitudes));
    return std::acos(cosAngle);
}

Vector3 Vector3::Project(const Vector3& a, const Vector3& b)
{
    float bMagnitudeSquared = b.MagnitudeSquared();
    if (bMagnitudeSquared < std::numeric_limits<float>::epsilon())
        return Vector3::Zero();

    float scalar = Dot(a, b) / bMagnitudeSquared;
    return b * scalar;
}

Vector3 Vector3::Reflect(const Vector3& a, const Vector3& n)
{
    // Formula: r = a - 2 * (a · n) * n
    return a - n * (2.0f * Dot(a, n));
}

float Vector3::ScalarTripleProduct(const Vector3& a, const Vector3& b, const Vector3& c)
{
    return Dot(a, Cross(b, c));
}

Vector3 Vector3::VectorTripleProduct(const Vector3& a, const Vector3& b, const Vector3& c)
{
    return Cross(a, Cross(b, c));
}

Vector3 Vector3::Slerp(const Vector3& a, const Vector3& b, float t)
{
    float dotProduct = Dot(a.Normalized(), b.Normalized());

    // If vectors are nearly parallel, use linear interpolation
    if (std::abs(dotProduct) > 0.9995f)
    {
        return Lerp(a, b, t).Normalized();
    }

    float angle = std::acos(std::abs(dotProduct));
    float sinAngle = std::sin(angle);

    if (sinAngle < std::numeric_limits<float>::epsilon())
    {
        return Lerp(a, b, t).Normalized();
    }

    float factorA = std::sin((1.0f - t) * angle) / sinAngle;
    float factorB = std::sin(t * angle) / sinAngle;

    if (dotProduct < 0.0f)
    {
        factorB = -factorB;
    }

    return (a * factorA + b * factorB).Normalized();
}

Vector3 Vector3::Orthogonal(const Vector3& v)
{
    // Choose the axis that is most perpendicular to v
    Vector3 axis = (std::abs(v.x) < std::abs(v.y)) ? ((std::abs(v.x) < std::abs(v.z)) ? Vector3(1.0f, 0.0f, 0.0f) : Vector3(0.0f, 0.0f, 1.0f)) : ((std::abs(v.y) < std::abs(v.z)) ? Vector3(0.0f, 1.0f, 0.0f) : Vector3(0.0f, 0.0f, 1.0f));

    return Cross(v, axis);
}

Vector3 Vector3::RotateAroundAxis(const Vector3& v, const Vector3& axis, float angle)
{
    // Rodrigues' rotation formula
    Vector3 normalizedAxis = axis.Normalized();
    float cosAngle = std::cos(angle);
    float sinAngle = std::sin(angle);

    return v * cosAngle +
           Cross(normalizedAxis, v) * sinAngle +
           normalizedAxis * Dot(normalizedAxis, v) * (1.0f - cosAngle);
}

// Static constant vectors
Vector3 Vector3::Zero()
{
    return Vector3(0.0f, 0.0f, 0.0f);
}

Vector3 Vector3::One()
{
    return Vector3(1.0f, 1.0f, 1.0f);
}

Vector3 Vector3::Up()
{
    return Vector3(0.0f, 1.0f, 0.0f);
}

Vector3 Vector3::Down()
{
    return Vector3(0.0f, -1.0f, 0.0f);
}

Vector3 Vector3::Left()
{
    return Vector3(-1.0f, 0.0f, 0.0f);
}

Vector3 Vector3::Right()
{
    return Vector3(1.0f, 0.0f, 0.0f);
}

Vector3 Vector3::Forward()
{
    return Vector3(0.0f, 0.0f, 1.0f);
}

Vector3 Vector3::Back()
{
    return Vector3(0.0f, 0.0f, -1.0f);
}

// Non-member operators
Vector3 operator*(float scalar, const Vector3& vector)
{
    return vector * scalar;
}

} // namespace Math