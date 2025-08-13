#pragma once

#include "Mat4.h"
#include <cmath>

namespace Math
{
    /**
     * @brief Creates a perspective projection matrix.
     * 
     * @param fovRadians The field of view in radians.
     * @param aspectRatio The aspect ratio of the viewport (width / height).
     * @param nearPlane The distance to the near clipping plane.
     * @param farPlane The distance to the far clipping plane.
     * @return A 4x4 perspective projection matrix.
     */
    inline Mat4 perspective(float fovRadians, float aspectRatio, float nearPlane, float farPlane)
    {
        Mat4 result;
        const float tanHalfFov = std::tan(fovRadians / 2.0f);

        result.columns[0][0] = 1.0f / (aspectRatio * tanHalfFov);
        result.columns[1][1] = 1.0f / (tanHalfFov);
        result.columns[2][2] = -(farPlane + nearPlane) / (farPlane - nearPlane);
        result.columns[2][3] = -1.0f;
        result.columns[3][2] = -(2.0f * farPlane * nearPlane) / (farPlane - nearPlane);
        result.columns[3][3] = 0.0f;

        return result;
    }
}
