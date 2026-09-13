#pragma once

#include "raylibUtils.h"

namespace SGL
{

    inline void EulerToQuat(
        float ex, float ey, float ez,
        float& qx, float& qy, float& qz, float& qw)
    {
        Quaternion Qx = QuaternionFromAxisAngle({ 1, 0, 0 }, ex);
        Quaternion Qy = QuaternionFromAxisAngle({ 0, 1, 0 }, ey);
        Quaternion Qz = QuaternionFromAxisAngle({ 0, 0, 1 }, ez);

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

        ex = rad.x;
        ey = rad.y;
        ez = rad.z;
    }

}