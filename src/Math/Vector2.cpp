#include "Vector2.h"

#include <cmath>

namespace Math
{

// Constructors
Vector2::Vector2()
    : x(0.0f), y(0.0f)
{
}

Vector2::Vector2(float x, float y)
    : x(x), y(y)
{
}

Vector2::Vector2(const Vector2& other)
    : x(other.x), y(other.y)
{
}

// Assignment operator
Vector2& Vector2::operator=(const Vector2& other)
{
    if (this != &other)
    {
        x = other.x;
        y = other.y;
    }
    return *this;
}

// Arithmetic operators
Vector2 Vector2::operator+(const Vector2& other) const
{
    return Vector2(x + other.x, y + other.y);
}

Vector2 Vector2::operator-(const Vector2& other) const
{
    return Vector2(x - other.x, y - other.y);
}

Vector2 Vector2::operator*(float scalar) const
{
    return Vector2(x * scalar, y * scalar);
}

Vector2 Vector2::operator/(float scalar) const
{
    return Vector2(x / scalar, y / scalar);
}

Vector2 Vector2::operator-() const
{
    return Vector2(-x, -y);
}

// Compound assignment operators
Vector2& Vector2::operator+=(const Vector2& other)
{
    x += other.x;
    y += other.y;
    return *this;
}

Vector2& Vector2::operator-=(const Vector2& other)
{
    x -= other.x;
    y -= other.y;
    return *this;
}

Vector2& Vector2::operator*=(float scalar)
{
    x *= scalar;
    y *= scalar;
    return *this;
}

Vector2& Vector2::operator/=(float scalar)
{
    x /= scalar;
    y /= scalar;
    return *this;
}

// Comparison operators
bool Vector2::operator==(const Vector2& other) const
{
    return (x == other.x) && (y == other.y);
}

bool Vector2::operator!=(const Vector2& other) const
{
    return !(*this == other);
}

// Vector operations
float Vector2::Magnitude() const
{
    return std::sqrt(x * x + y * y);
}

float Vector2::MagnitudeSquared() const
{
    return x * x + y * y;
}

void Vector2::Normalize()
{
    float mag = Magnitude();
    if (mag > 0)
    {
        x /= mag;
        y /= mag;
    }
}

Vector2 Vector2::Normalized() const
{
    Vector2 result(*this);
    result.Normalize();
    return result;
}

// Static utility functions
float Vector2::Dot(const Vector2& a, const Vector2& b)
{
    return a.x * b.x + a.y * b.y;
}

float Vector2::Cross(const Vector2& a, const Vector2& b)
{
    return a.x * b.y - a.y * b.x;
}

float Vector2::Distance(const Vector2& a, const Vector2& b)
{
    return (b - a).Magnitude();
}

float Vector2::DistanceSquared(const Vector2& a, const Vector2& b)
{
    return (b - a).MagnitudeSquared();
}

Vector2 Vector2::Lerp(const Vector2& a, const Vector2& b, float t)
{
    return a + (b - a) * t;
}

Vector2 Vector2::LerpClamped(const Vector2& a, const Vector2& b, float t)
{
    if (t < 0.0f)
        t = 0.0f;
    if (t > 1.0f)
        t = 1.0f;
    return Lerp(a, b, t);
}

float Vector2::Angle(const Vector2& a, const Vector2& b)
{
    float dot = Dot(a.Normalized(), b.Normalized());
    // Clamp to handle floating point errors
    if (dot < -1.0f)
        dot = -1.0f;
    if (dot > 1.0f)
        dot = 1.0f;
    return std::acos(dot);
}

Vector2 Vector2::Project(const Vector2& a, const Vector2& b)
{
    float dot = Dot(a, b);
    float magSq = b.MagnitudeSquared();
    if (magSq > 0.0f)
    {
        return b * (dot / magSq);
    }
    return Zero();
}

Vector2 Vector2::Reflect(const Vector2& a, const Vector2& n)
{
    return a - n * (2.0f * Dot(a, n));
}

// Static constant vectors
Vector2 Vector2::Zero()
{
    return Vector2(0.0f, 0.0f);
}

Vector2 Vector2::One()
{
    return Vector2(1.0f, 1.0f);
}

Vector2 Vector2::Up()
{
    return Vector2(0.0f, 1.0f);
}

Vector2 Vector2::Down()
{
    return Vector2(0.0f, -1.0f);
}

Vector2 Vector2::Left()
{
    return Vector2(-1.0f, 0.0f);
}

Vector2 Vector2::Right()
{
    return Vector2(1.0f, 0.0f);
}

// Non-member operators for scalar multiplication (scalar * vector)
Vector2 operator*(float scalar, const Vector2& vector)
{
    return vector * scalar;
}

} // namespace Math