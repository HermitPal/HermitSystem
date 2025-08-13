#pragma once

#include "Mat4.h"
#include "Vec3.h"
#include <cmath>

namespace Math
{
    inline Mat4 translate(const Mat4& m, const Vec3& v)
    {
        Mat4 result = m;
        result.columns[3] = m.columns[0] * v.x + m.columns[1] * v.y + m.columns[2] * v.z + m.columns[3];
        return result;
    }

    inline Mat4 scale(const Mat4& m, const Vec3& v)
    {
        Mat4 result = m;
        result.columns[0] = m.columns[0] * v.x;
        result.columns[1] = m.columns[1] * v.y;
        result.columns[2] = m.columns[2] * v.z;
        return result;
    }

    inline Mat4 rotate(const Mat4& m, float angleRadians, const Vec3& axis)
    {
        Mat4 result;
        const Vec3 a = normalize(axis);
        const float s = std::sin(angleRadians);
        const float c = std::cos(angleRadians);
        const float oc = 1.0f - c;

        result.columns[0].x = c + a.x * a.x * oc;
        result.columns[0].y = a.y * a.x * oc + a.z * s;
        result.columns[0].z = a.z * a.x * oc - a.y * s;

        result.columns[1].x = a.x * a.y * oc - a.z * s;
        result.columns[1].y = c + a.y * a.y * oc;
        result.columns[1].z = a.z * a.y * oc + a.x * s;

        result.columns[2].x = a.x * a.z * oc + a.y * s;
        result.columns[2].y = a.y * a.z * oc - a.x * s;
        result.columns[2].z = c + a.z * a.z * oc;

        return m * result;
    }
}
