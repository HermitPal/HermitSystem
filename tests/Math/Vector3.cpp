#include "Math/Vector3.h"
#include <cmath>
#include <gtest/gtest.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

using namespace Math;

class Vector3Test : public ::testing::Test
{
  protected:
    void SetUp() override
    {
        // Common test vectors
        zero = Vector3(0.0f, 0.0f, 0.0f);
        unitX = Vector3(1.0f, 0.0f, 0.0f);
        unitY = Vector3(0.0f, 1.0f, 0.0f);
        unitZ = Vector3(0.0f, 0.0f, 1.0f);
        diagonal = Vector3(3.0f, 4.0f, 5.0f);
        negative = Vector3(-2.0f, -5.0f, -1.0f);
        mixed = Vector3(1.0f, -2.0f, 3.0f);
    }

    Vector3 zero, unitX, unitY, unitZ, diagonal, negative, mixed;
    const float EPSILON = 1e-6f;
};

// Constructor Tests
TEST_F(Vector3Test, DefaultConstructor)
{
    Vector3 v;
    EXPECT_FLOAT_EQ(v.x, 0.0f);
    EXPECT_FLOAT_EQ(v.y, 0.0f);
    EXPECT_FLOAT_EQ(v.z, 0.0f);
}

TEST_F(Vector3Test, ParameterizedConstructor)
{
    Vector3 v(1.5f, -2.3f, 4.7f);
    EXPECT_FLOAT_EQ(v.x, 1.5f);
    EXPECT_FLOAT_EQ(v.y, -2.3f);
    EXPECT_FLOAT_EQ(v.z, 4.7f);
}

TEST_F(Vector3Test, CopyConstructor)
{
    Vector3 v(diagonal);
    EXPECT_FLOAT_EQ(v.x, 3.0f);
    EXPECT_FLOAT_EQ(v.y, 4.0f);
    EXPECT_FLOAT_EQ(v.z, 5.0f);
}

// Assignment Operator Tests
TEST_F(Vector3Test, AssignmentOperator)
{
    Vector3 v;
    v = diagonal;
    EXPECT_FLOAT_EQ(v.x, 3.0f);
    EXPECT_FLOAT_EQ(v.y, 4.0f);
    EXPECT_FLOAT_EQ(v.z, 5.0f);
}

// Arithmetic Operator Tests
TEST_F(Vector3Test, Addition)
{
    Vector3 result = unitX + unitY;
    EXPECT_FLOAT_EQ(result.x, 1.0f);
    EXPECT_FLOAT_EQ(result.y, 1.0f);
    EXPECT_FLOAT_EQ(result.z, 0.0f);

    result = diagonal + negative;
    EXPECT_FLOAT_EQ(result.x, 1.0f);
    EXPECT_FLOAT_EQ(result.y, -1.0f);
    EXPECT_FLOAT_EQ(result.z, 4.0f);
}

TEST_F(Vector3Test, Subtraction)
{
    Vector3 result = diagonal - unitX;
    EXPECT_FLOAT_EQ(result.x, 2.0f);
    EXPECT_FLOAT_EQ(result.y, 4.0f);
    EXPECT_FLOAT_EQ(result.z, 5.0f);

    result = unitZ - negative;
    EXPECT_FLOAT_EQ(result.x, 2.0f);
    EXPECT_FLOAT_EQ(result.y, 5.0f);
    EXPECT_FLOAT_EQ(result.z, 2.0f);
}

TEST_F(Vector3Test, ScalarMultiplication)
{
    Vector3 result = diagonal * 2.0f;
    EXPECT_FLOAT_EQ(result.x, 6.0f);
    EXPECT_FLOAT_EQ(result.y, 8.0f);
    EXPECT_FLOAT_EQ(result.z, 10.0f);

    result = negative * -1.5f;
    EXPECT_FLOAT_EQ(result.x, 3.0f);
    EXPECT_FLOAT_EQ(result.y, 7.5f);
    EXPECT_FLOAT_EQ(result.z, 1.5f);
}

TEST_F(Vector3Test, ScalarMultiplicationCommutative)
{
    Vector3 result1 = diagonal * 3.0f;
    Vector3 result2 = 3.0f * diagonal;
    EXPECT_FLOAT_EQ(result1.x, result2.x);
    EXPECT_FLOAT_EQ(result1.y, result2.y);
    EXPECT_FLOAT_EQ(result1.z, result2.z);
}

TEST_F(Vector3Test, ScalarDivision)
{
    Vector3 result = diagonal / 2.0f;
    EXPECT_FLOAT_EQ(result.x, 1.5f);
    EXPECT_FLOAT_EQ(result.y, 2.0f);
    EXPECT_FLOAT_EQ(result.z, 2.5f);
}

TEST_F(Vector3Test, UnaryMinus)
{
    Vector3 result = -diagonal;
    EXPECT_FLOAT_EQ(result.x, -3.0f);
    EXPECT_FLOAT_EQ(result.y, -4.0f);
    EXPECT_FLOAT_EQ(result.z, -5.0f);
}

// Compound Assignment Operator Tests
TEST_F(Vector3Test, CompoundAddition)
{
    Vector3 v = unitX;
    v += unitY;
    EXPECT_FLOAT_EQ(v.x, 1.0f);
    EXPECT_FLOAT_EQ(v.y, 1.0f);
    EXPECT_FLOAT_EQ(v.z, 0.0f);
}

TEST_F(Vector3Test, CompoundSubtraction)
{
    Vector3 v = diagonal;
    v -= unitZ;
    EXPECT_FLOAT_EQ(v.x, 3.0f);
    EXPECT_FLOAT_EQ(v.y, 4.0f);
    EXPECT_FLOAT_EQ(v.z, 4.0f);
}

TEST_F(Vector3Test, CompoundScalarMultiplication)
{
    Vector3 v = diagonal;
    v *= 0.5f;
    EXPECT_FLOAT_EQ(v.x, 1.5f);
    EXPECT_FLOAT_EQ(v.y, 2.0f);
    EXPECT_FLOAT_EQ(v.z, 2.5f);
}

TEST_F(Vector3Test, CompoundScalarDivision)
{
    Vector3 v = diagonal;
    v /= 3.0f;
    EXPECT_NEAR(v.x, 1.0f, EPSILON);
    EXPECT_NEAR(v.y, 4.0f / 3.0f, EPSILON);
    EXPECT_NEAR(v.z, 5.0f / 3.0f, EPSILON);
}

// Comparison Operator Tests
TEST_F(Vector3Test, Equality)
{
    Vector3 v1(1.0f, 2.0f, 3.0f);
    Vector3 v2(1.0f, 2.0f, 3.0f);
    EXPECT_TRUE(v1 == v2);
    EXPECT_FALSE(v1 == diagonal);
}

TEST_F(Vector3Test, Inequality)
{
    EXPECT_TRUE(diagonal != negative);
    EXPECT_FALSE(unitX != unitX);
}

// Magnitude Tests
TEST_F(Vector3Test, MagnitudeZero)
{
    EXPECT_FLOAT_EQ(zero.Magnitude(), 0.0f);
}

TEST_F(Vector3Test, MagnitudeUnit)
{
    EXPECT_FLOAT_EQ(unitX.Magnitude(), 1.0f);
    EXPECT_FLOAT_EQ(unitY.Magnitude(), 1.0f);
    EXPECT_FLOAT_EQ(unitZ.Magnitude(), 1.0f);
}

TEST_F(Vector3Test, MagnitudeDiagonal)
{
    // sqrt(3^2 + 4^2 + 5^2) = sqrt(9 + 16 + 25) = sqrt(50) ≈ 7.071
    EXPECT_NEAR(diagonal.Magnitude(), std::sqrt(50.0f), EPSILON);
}

TEST_F(Vector3Test, MagnitudeSquared)
{
    EXPECT_FLOAT_EQ(zero.MagnitudeSquared(), 0.0f);
    EXPECT_FLOAT_EQ(unitX.MagnitudeSquared(), 1.0f);
    EXPECT_FLOAT_EQ(diagonal.MagnitudeSquared(), 50.0f);
}

// Normalization Tests
TEST_F(Vector3Test, NormalizeInPlace)
{
    Vector3 v = diagonal;
    v.Normalize();
    EXPECT_NEAR(v.Magnitude(), 1.0f, EPSILON);
    EXPECT_NEAR(v.x, 3.0f / std::sqrt(50.0f), EPSILON);
    EXPECT_NEAR(v.y, 4.0f / std::sqrt(50.0f), EPSILON);
    EXPECT_NEAR(v.z, 5.0f / std::sqrt(50.0f), EPSILON);
}

TEST_F(Vector3Test, NormalizeZeroVector)
{
    Vector3 v = zero;
    v.Normalize();
    EXPECT_FLOAT_EQ(v.x, 0.0f);
    EXPECT_FLOAT_EQ(v.y, 0.0f);
    EXPECT_FLOAT_EQ(v.z, 0.0f);
}

TEST_F(Vector3Test, NormalizedCopy)
{
    Vector3 normalized = diagonal.Normalized();
    EXPECT_NEAR(normalized.Magnitude(), 1.0f, EPSILON);
    // Original should remain unchanged
    EXPECT_FLOAT_EQ(diagonal.x, 3.0f);
    EXPECT_FLOAT_EQ(diagonal.y, 4.0f);
    EXPECT_FLOAT_EQ(diagonal.z, 5.0f);
}

// Dot Product Tests
TEST_F(Vector3Test, DotProductOrthogonal)
{
    EXPECT_FLOAT_EQ(Vector3::Dot(unitX, unitY), 0.0f);
    EXPECT_FLOAT_EQ(Vector3::Dot(unitX, unitZ), 0.0f);
    EXPECT_FLOAT_EQ(Vector3::Dot(unitY, unitZ), 0.0f);
}

TEST_F(Vector3Test, DotProductParallel)
{
    EXPECT_FLOAT_EQ(Vector3::Dot(unitX, unitX), 1.0f);
    EXPECT_FLOAT_EQ(Vector3::Dot(diagonal, diagonal), 50.0f);
}

TEST_F(Vector3Test, DotProductGeneral)
{
    Vector3 a(1.0f, 2.0f, 3.0f);
    Vector3 b(4.0f, 5.0f, 6.0f);
    // 1*4 + 2*5 + 3*6 = 4 + 10 + 18 = 32
    EXPECT_FLOAT_EQ(Vector3::Dot(a, b), 32.0f);
}

// Cross Product Tests
TEST_F(Vector3Test, CrossProductBasis)
{
    Vector3 result = Vector3::Cross(unitX, unitY);
    EXPECT_NEAR(result.x, 0.0f, EPSILON);
    EXPECT_NEAR(result.y, 0.0f, EPSILON);
    EXPECT_NEAR(result.z, 1.0f, EPSILON);

    result = Vector3::Cross(unitY, unitZ);
    EXPECT_NEAR(result.x, 1.0f, EPSILON);
    EXPECT_NEAR(result.y, 0.0f, EPSILON);
    EXPECT_NEAR(result.z, 0.0f, EPSILON);

    result = Vector3::Cross(unitZ, unitX);
    EXPECT_NEAR(result.x, 0.0f, EPSILON);
    EXPECT_NEAR(result.y, 1.0f, EPSILON);
    EXPECT_NEAR(result.z, 0.0f, EPSILON);
}

TEST_F(Vector3Test, CrossProductAnticommutative)
{
    Vector3 a(1.0f, 2.0f, 3.0f);
    Vector3 b(4.0f, 5.0f, 6.0f);
    Vector3 cross1 = Vector3::Cross(a, b);
    Vector3 cross2 = Vector3::Cross(b, a);

    EXPECT_NEAR(cross1.x, -cross2.x, EPSILON);
    EXPECT_NEAR(cross1.y, -cross2.y, EPSILON);
    EXPECT_NEAR(cross1.z, -cross2.z, EPSILON);
}

TEST_F(Vector3Test, CrossProductParallelVectors)
{
    Vector3 result = Vector3::Cross(unitX, unitX);
    EXPECT_NEAR(result.x, 0.0f, EPSILON);
    EXPECT_NEAR(result.y, 0.0f, EPSILON);
    EXPECT_NEAR(result.z, 0.0f, EPSILON);
}

// Distance Tests
TEST_F(Vector3Test, Distance)
{
    EXPECT_FLOAT_EQ(Vector3::Distance(zero, unitX), 1.0f);
    EXPECT_NEAR(Vector3::Distance(zero, diagonal), std::sqrt(50.0f), EPSILON);
}

TEST_F(Vector3Test, DistanceSquared)
{
    EXPECT_FLOAT_EQ(Vector3::DistanceSquared(zero, unitX), 1.0f);
    EXPECT_FLOAT_EQ(Vector3::DistanceSquared(zero, diagonal), 50.0f);
}

// Interpolation Tests
TEST_F(Vector3Test, Lerp)
{
    Vector3 result = Vector3::Lerp(zero, diagonal, 0.5f);
    EXPECT_FLOAT_EQ(result.x, 1.5f);
    EXPECT_FLOAT_EQ(result.y, 2.0f);
    EXPECT_FLOAT_EQ(result.z, 2.5f);

    result = Vector3::Lerp(unitX, unitY, 0.0f);
    EXPECT_FLOAT_EQ(result.x, 1.0f);
    EXPECT_FLOAT_EQ(result.y, 0.0f);
    EXPECT_FLOAT_EQ(result.z, 0.0f);

    result = Vector3::Lerp(unitX, unitY, 1.0f);
    EXPECT_FLOAT_EQ(result.x, 0.0f);
    EXPECT_FLOAT_EQ(result.y, 1.0f);
    EXPECT_FLOAT_EQ(result.z, 0.0f);
}

TEST_F(Vector3Test, LerpClamped)
{
    Vector3 result = Vector3::LerpClamped(zero, diagonal, -0.5f);
    EXPECT_FLOAT_EQ(result.x, 0.0f);
    EXPECT_FLOAT_EQ(result.y, 0.0f);
    EXPECT_FLOAT_EQ(result.z, 0.0f);

    result = Vector3::LerpClamped(zero, diagonal, 1.5f);
    EXPECT_FLOAT_EQ(result.x, 3.0f);
    EXPECT_FLOAT_EQ(result.y, 4.0f);
    EXPECT_FLOAT_EQ(result.z, 5.0f);
}

// Angle Tests
TEST_F(Vector3Test, AngleOrthogonal)
{
    float angle = Vector3::Angle(unitX, unitY);
    EXPECT_NEAR(angle, static_cast<float>(M_PI) / 2.0f, EPSILON);
}

TEST_F(Vector3Test, AngleParallel)
{
    float angle = Vector3::Angle(unitX, unitX);
    EXPECT_NEAR(angle, 0.0f, EPSILON);

    angle = Vector3::Angle(unitX, -unitX);
    EXPECT_NEAR(angle, static_cast<float>(M_PI), EPSILON);
}

// Projection Tests
TEST_F(Vector3Test, Project)
{
    Vector3 result = Vector3::Project(diagonal, unitX);
    EXPECT_NEAR(result.x, 3.0f, EPSILON);
    EXPECT_NEAR(result.y, 0.0f, EPSILON);
    EXPECT_NEAR(result.z, 0.0f, EPSILON);
}

// Reflection Tests
TEST_F(Vector3Test, Reflect)
{
    Vector3 incident(-1.0f, -1.0f, 0.0f);
    Vector3 normal(0.0f, 1.0f, 0.0f);
    Vector3 reflected = Vector3::Reflect(incident, normal);
    EXPECT_NEAR(reflected.x, -1.0f, EPSILON);
    EXPECT_NEAR(reflected.y, 1.0f, EPSILON);
    EXPECT_NEAR(reflected.z, 0.0f, EPSILON);
}

// Static Vector Tests
TEST_F(Vector3Test, StaticVectors)
{
    Vector3 zeroVec = Vector3::Zero();
    EXPECT_FLOAT_EQ(zeroVec.x, 0.0f);
    EXPECT_FLOAT_EQ(zeroVec.y, 0.0f);
    EXPECT_FLOAT_EQ(zeroVec.z, 0.0f);

    Vector3 oneVec = Vector3::One();
    EXPECT_FLOAT_EQ(oneVec.x, 1.0f);
    EXPECT_FLOAT_EQ(oneVec.y, 1.0f);
    EXPECT_FLOAT_EQ(oneVec.z, 1.0f);

    Vector3 upVec = Vector3::Up();
    EXPECT_FLOAT_EQ(upVec.x, 0.0f);
    EXPECT_FLOAT_EQ(upVec.y, 1.0f);
    EXPECT_FLOAT_EQ(upVec.z, 0.0f);

    Vector3 forwardVec = Vector3::Forward();
    EXPECT_FLOAT_EQ(forwardVec.x, 0.0f);
    EXPECT_FLOAT_EQ(forwardVec.y, 0.0f);
    EXPECT_FLOAT_EQ(forwardVec.z, 1.0f);
}