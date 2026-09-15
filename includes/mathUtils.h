#pragma once

#include "raylibUtils.h"

namespace SGL
{

    inline void EulerToQuat(
        float ex, float ey, float ez,
        float& qx, float& qy, float& qz, float& qw)
    {
        Quaternion Q = QuaternionFromEuler(ex, ey, ez);

        qx = Q.x; qy = Q.y; qz = Q.z; qw = Q.w;
    }

    inline void QuatToEuler(
        float qx, float qy, float qz, float qw,
        float& ex, float& ey, float& ez)
    {
        Quaternion Q = { qx, qy, qz, qw };
        Vector3 rad = QuaternionToEuler(Q);

        ex = rad.x;
        ey = rad.y;
        ez = rad.z;
    }

}