#pragma once

#include "raylibUtils.h"
#include "mathUtils.h"

namespace SGL
{

    struct InternalCamera
    {
        Camera3D raylibCamera = {};

        float fov = 60.0f;
        Vector3 position = { 0, 0, 0 };
        Quaternion orientation = QuaternionIdentity();

        float nearPlane = 0.01f;
        float farPlane = 1000.0f;

        Vector3 GetForward() const;
        Vector3 GetUp() const;

        void SetRotationEuler(Vector3 deg);
        Vector3 GetRotationEuler() const;

        void SetPosition(float x, float y, float z);
        Vector3 GetPosition() const;

        void SetNearFarPlane(float near, float far);
        void GetNearFarPlane(float* near, float* far);

        void UpdateCamera();
    };

    InternalCamera& GetCameraInstance();

}