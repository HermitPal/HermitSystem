#include "Math/Vector2.h"
#include <cmath>
#include <gtest/gtest.h>

using namespace Math; // Assuming Vector2 is in Math namespace

// Test fixture for Vector2 tests
class Vector2Test : public ::testing::Test
{
  protected:
    void SetUp() override
    {
        // Common test vectors
        zero = Vector2(0.0f, 0.0f);
        unitX = Vector2(1.0f, 0.0f);
        unitY = Vector2(0.0f, 1.0f);
        diagonal = Vector2(3.0f, 4.0f);
        negative = Vector2(-2.0f, -5.0f);
    }

    Vector2 zero, unitX, unitY, diagonal, negative;
    const float EPSILON = 1e-6f;
};

// Basic construction and accessors
TEST_F(Vector2Test, DefaultConstruction)
{
    Vector2 v;
    EXPECT_FLOAT_EQ(v.x, 0.0f);
    EXPECT_FLOAT_EQ(v.y, 0.0f);
}

TEST_F(Vector2Test, ParameterizedConstruction)
{
    Vector2 v(3.5f, -2.1f);
    EXPECT_FLOAT_EQ(v.x, 3.5f);
    EXPECT_FLOAT_EQ(v.y, -2.1f);
}

TEST_F(Vector2Test, CopyConstruction)
{
    Vector2 original(5.0f, 7.0f);
    Vector2 copy(original);

    EXPECT_FLOAT_EQ(copy.x, 5.0f);
    EXPECT_FLOAT_EQ(copy.y, 7.0f);
}

// Arithmetic operations
TEST_F(Vector2Test, VectorAddition)
{
    Vector2 result = diagonal + negative;

    EXPECT_FLOAT_EQ(result.x, 1.0f);
    EXPECT_FLOAT_EQ(result.y, -1.0f);
}

TEST_F(Vector2Test, VectorSubtraction)
{
    Vector2 result = diagonal - unitX;

    EXPECT_FLOAT_EQ(result.x, 2.0f);
    EXPECT_FLOAT_EQ(result.y, 4.0f);
}

TEST_F(Vector2Test, ScalarMultiplication)
{
    Vector2 result = diagonal * 2.0f;

    EXPECT_FLOAT_EQ(result.x, 6.0f);
    EXPECT_FLOAT_EQ(result.y, 8.0f);
}

TEST_F(Vector2Test, ScalarDivision)
{
    Vector2 result = diagonal / 2.0f;

    EXPECT_FLOAT_EQ(result.x, 1.5f);
    EXPECT_FLOAT_EQ(result.y, 2.0f);
}

TEST_F(Vector2Test, UnaryMinus)
{
    Vector2 result = -diagonal;

    EXPECT_FLOAT_EQ(result.x, -3.0f);
    EXPECT_FLOAT_EQ(result.y, -4.0f);
}

// Assignment operations
TEST_F(Vector2Test, AdditionAssignment)
{
    Vector2 v = diagonal;
    v += unitX;

    EXPECT_FLOAT_EQ(v.x, 4.0f);
    EXPECT_FLOAT_EQ(v.y, 4.0f);
}

TEST_F(Vector2Test, SubtractionAssignment)
{
    Vector2 v = diagonal;
    v -= unitY;

    EXPECT_FLOAT_EQ(v.x, 3.0f);
    EXPECT_FLOAT_EQ(v.y, 3.0f);
}

TEST_F(Vector2Test, ScalarMultiplicationAssignment)
{
    Vector2 v = diagonal;
    v *= 0.5f;

    EXPECT_FLOAT_EQ(v.x, 1.5f);
    EXPECT_FLOAT_EQ(v.y, 2.0f);
}

TEST_F(Vector2Test, ScalarDivisionAssignment)
{
    Vector2 v = diagonal;
    v /= 2.0f;

    EXPECT_FLOAT_EQ(v.x, 1.5f);
    EXPECT_FLOAT_EQ(v.y, 2.0f);
}

// Comparison operations
TEST_F(Vector2Test, Equality)
{
    Vector2 v1(1.0f, 2.0f);
    Vector2 v2(1.0f, 2.0f);
    Vector2 v3(1.1f, 2.0f);

    EXPECT_TRUE(v1 == v2);
    EXPECT_FALSE(v1 == v3);
}

TEST_F(Vector2Test, Inequality)
{
    Vector2 v1(1.0f, 2.0f);
    Vector2 v2(1.1f, 2.0f);

    EXPECT_TRUE(v1 != v2);
    EXPECT_FALSE(v1 != v1);
}

// Mathematical properties
TEST_F(Vector2Test, Magnitude)
{
    EXPECT_FLOAT_EQ(zero.Magnitude(), 0.0f);
    EXPECT_FLOAT_EQ(unitX.Magnitude(), 1.0f);
    EXPECT_FLOAT_EQ(unitY.Magnitude(), 1.0f);
    EXPECT_FLOAT_EQ(diagonal.Magnitude(), 5.0f); // 3-4-5 triangle
}

TEST_F(Vector2Test, MagnitudeSquared)
{
    EXPECT_FLOAT_EQ(zero.MagnitudeSquared(), 0.0f);
    EXPECT_FLOAT_EQ(unitX.MagnitudeSquared(), 1.0f);
    EXPECT_FLOAT_EQ(diagonal.MagnitudeSquared(), 25.0f); // 3² + 4² = 25
}

TEST_F(Vector2Test, Normalize)
{
    Vector2 v = diagonal;
    v.Normalize();

    EXPECT_NEAR(v.Magnitude(), 1.0f, EPSILON);
    EXPECT_FLOAT_EQ(v.x, 0.6f); // 3/5
    EXPECT_FLOAT_EQ(v.y, 0.8f); // 4/5
}

TEST_F(Vector2Test, NormalizeZeroVector)
{
    Vector2 v = zero;
    v.Normalize();

    // Zero vector normalization should remain zero
    EXPECT_FLOAT_EQ(v.x, 0.0f);
    EXPECT_FLOAT_EQ(v.y, 0.0f);
}

TEST_F(Vector2Test, Normalized)
{
    Vector2 original = diagonal;
    Vector2 normalized = original.Normalized();

    // Original should be unchanged
    EXPECT_FLOAT_EQ(original.x, 3.0f);
    EXPECT_FLOAT_EQ(original.y, 4.0f);

    // Normalized should have unit length
    EXPECT_NEAR(normalized.Magnitude(), 1.0f, EPSILON);
    EXPECT_FLOAT_EQ(normalized.x, 0.6f);
    EXPECT_FLOAT_EQ(normalized.y, 0.8f);
}

TEST_F(Vector2Test, DotProduct)
{
    float dot1 = Vector2::Dot(unitX, unitY);
    float dot2 = Vector2::Dot(diagonal, Vector2(1.0f, 1.0f));
    float dot3 = Vector2::Dot(diagonal, diagonal);

    EXPECT_FLOAT_EQ(dot1, 0.0f);  // Perpendicular vectors
    EXPECT_FLOAT_EQ(dot2, 7.0f);  // 3*1 + 4*1 = 7
    EXPECT_FLOAT_EQ(dot3, 25.0f); // 3*3 + 4*4 = 25
}

TEST_F(Vector2Test, CrossProduct)
{
    float cross1 = Vector2::Cross(unitX, unitY);
    float cross2 = Vector2::Cross(unitY, unitX);
    float cross3 = Vector2::Cross(diagonal, Vector2(4.0f, -3.0f));

    EXPECT_FLOAT_EQ(cross1, 1.0f);   // Standard basis vectors
    EXPECT_FLOAT_EQ(cross2, -1.0f);  // Reversed order
    EXPECT_FLOAT_EQ(cross3, -25.0f); // 3*(-3) - 4*4 = -25
}

TEST_F(Vector2Test, Distance)
{
    float dist1 = Vector2::Distance(zero, diagonal);
    float dist2 = Vector2::Distance(unitX, unitY);

    EXPECT_FLOAT_EQ(dist1, 5.0f);
    EXPECT_NEAR(dist2, std::sqrt(2.0f), EPSILON);
}

TEST_F(Vector2Test, DistanceSquared)
{
    float distSq1 = Vector2::DistanceSquared(zero, diagonal);
    float distSq2 = Vector2::DistanceSquared(unitX, unitY);

    EXPECT_FLOAT_EQ(distSq1, 25.0f);
    EXPECT_FLOAT_EQ(distSq2, 2.0f);
}

TEST_F(Vector2Test, Lerp)
{
    Vector2 result1 = Vector2::Lerp(zero, diagonal, 0.5f);
    Vector2 result2 = Vector2::Lerp(unitX, unitY, 0.0f);
    Vector2 result3 = Vector2::Lerp(unitX, unitY, 1.0f);

    EXPECT_FLOAT_EQ(result1.x, 1.5f);
    EXPECT_FLOAT_EQ(result1.y, 2.0f);

    EXPECT_FLOAT_EQ(result2.x, 1.0f);
    EXPECT_FLOAT_EQ(result2.y, 0.0f);

    EXPECT_FLOAT_EQ(result3.x, 0.0f);
    EXPECT_FLOAT_EQ(result3.y, 1.0f);
}

// Edge cases and special values
TEST_F(Vector2Test, ZeroDivision)
{
    Vector2 v(2.0f, 4.0f);
    Vector2 result = v / 0.0f;

    // Should handle division by zero gracefully
    EXPECT_TRUE(std::isinf(result.x) || std::isnan(result.x));
    EXPECT_TRUE(std::isinf(result.y) || std::isnan(result.y));
}

TEST_F(Vector2Test, VerySmallNumbers)
{
    Vector2 tiny(1e-10f, 1e-10f);
    Vector2 normalized = tiny.Normalized();

    // Should handle very small numbers
    if (normalized.Magnitude() > 0)
    {
        EXPECT_NEAR(normalized.Magnitude(), 1.0f, EPSILON);
    }
}

// Static utility functions
TEST_F(Vector2Test, StaticConstants)
{
    Vector2 zeroVec = Vector2::Zero();
    Vector2 oneVec = Vector2::One();
    Vector2 upVec = Vector2::Up();
    Vector2 rightVec = Vector2::Right();

    EXPECT_FLOAT_EQ(zeroVec.x, 0.0f);
    EXPECT_FLOAT_EQ(zeroVec.y, 0.0f);

    EXPECT_FLOAT_EQ(oneVec.x, 1.0f);
    EXPECT_FLOAT_EQ(oneVec.y, 1.0f);

    EXPECT_FLOAT_EQ(upVec.x, 0.0f);
    EXPECT_FLOAT_EQ(upVec.y, 1.0f);

    EXPECT_FLOAT_EQ(rightVec.x, 1.0f);
    EXPECT_FLOAT_EQ(rightVec.y, 0.0f);
}
