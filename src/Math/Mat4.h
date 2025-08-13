#pragma once

#include "Vec3.h"
#include "Vec4.h"

namespace Math
{
    /**
     * @brief A 4x4 matrix, stored in column-major order.
     *
     * This matrix class is designed for 3D transformations. The memory layout
     * is column-major, which is compatible with OpenGL, Vulkan, and DirectX.
     * 
     * Memory Layout:
     * [col0.x, col0.y, col0.z, col0.w,  // First column
     *  col1.x, col1.y, col1.z, col1.w,  // Second column
     *  col2.x, col2.y, col2.z, col2.w,  // Third column
     *  col3.x, col3.y, col3.z, col3.w]  // Fourth column
     */
    struct Mat4
    {
        Vec4 columns[4];

        // Default constructor initializes to an identity matrix.
        Mat4()
        {
            columns[0] = { 1.0f, 0.0f, 0.0f, 0.0f };
            columns[1] = { 0.0f, 1.0f, 0.0f, 0.0f };
            columns[2] = { 0.0f, 0.0f, 1.0f, 0.0f };
            columns[3] = { 0.0f, 0.0f, 0.0f, 1.0f };
        }

        // Matrix-Matrix multiplication
        Mat4 operator*(const Mat4& other) const
        {
            Mat4 result;
            const Mat4& a = *this;
            const Mat4& b = other;

            for (int c = 0; c < 4; ++c)
            {
                result.columns[c] = 
                    a.columns[0] * b.columns[c].x +
                    a.columns[1] * b.columns[c].y +
                    a.columns[2] * b.columns[c].z +
                    a.columns[3] * b.columns[c].w;
            }

            return result;
        }
    };

}
