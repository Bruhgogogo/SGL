#pragma once

#include "core.h"
#include "mathUtils.h"

namespace SGL
{

    struct InternalCamera
    {
        raylib::Camera3D raylibCamera = {};

        float fov = 60.0f;
        raylib::Vector3 position = { 0, 0, 0 };
        raylib::Quaternion orientation = raylib::QuaternionIdentity();

        raylib::Vector3 GetForward() const;
        raylib::Vector3 GetUp() const;

        void SetRotationEuler(raylib::Vector3 deg);
        raylib::Vector3 GetRotationEuler() const;

        void SetPosition(float x, float y, float z);
        raylib::Vector3 GetPosition() const;

        void UpdateCamera();
    };

    InternalCamera& GetCameraInstance();

}