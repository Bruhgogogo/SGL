#pragma once

#include "raylibUtils.h"

namespace SGL
{

    constexpr float DEG_TO_RAD = PI / 180.0f;
    constexpr float RAD_TO_DEG = 180.0f / PI;

    inline void EulerToQuat(
        float ex, float ey, float ez,
        float& qx, float& qy, float& qz, float& qw)
    {
        float rx = ex * DEG_TO_RAD;
        float ry = ey * DEG_TO_RAD;
        float rz = ez * DEG_TO_RAD;

        Quaternion Qx = QuaternionFromAxisAngle({ 1, 0, 0 }, rx);
        Quaternion Qy = QuaternionFromAxisAngle({ 0, 1, 0 }, ry);
        Quaternion Qz = QuaternionFromAxisAngle({ 0, 0, 1 }, rz);

        Quaternion Q = QuaternionMultiply(Qz,
            QuaternionMultiply(Qy, Qx));
        Q = QuaternionNormalize(Q);

        qx = Q.x; qy = Q.y; qz = Q.z; qw = Q.w;
    }

    inline void QuatToEuler(
        float qx, float qy, float qz, float qw,
        float& ex, float& ey, float& ez)
    {
        Quaternion Q = { qx, qy, qz, qw };
        Vector3 rad = QuaternionToEuler(Q);

        ex = rad.x * RAD_TO_DEG;
        ey = rad.y * RAD_TO_DEG;
        ez = rad.z * RAD_TO_DEG;
    }

}