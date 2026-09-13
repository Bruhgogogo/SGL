#include "SGL.h"
#include "camera.h"

namespace SGL
{

    Vector3 InternalCamera::GetForward() const
    {
        return Vector3RotateByQuaternion({ 0.0f, 0.0f, -1.0f }, orientation);
    }

    Vector3 InternalCamera::GetUp() const
    {
        return Vector3RotateByQuaternion({ 0.0f, 1.0f, 0.0f }, orientation);
    }

    void InternalCamera::SetNearFarPlane(float near, float far)
    {
        nearPlane = near;
        farPlane = far;
        rlSetClipPlanes(near, far);
    }

    void InternalCamera::GetNearFarPlane(float* near, float* far)
    {
        *near = nearPlane;
        *far = farPlane;
    }

    void InternalCamera::SetRotationEuler(Vector3 deg)
    {
        float qx, qy, qz, qw;
        EulerToQuat(deg.x, deg.y, deg.z, qx, qy, qz, qw);
        orientation = { qx, qy, qz, qw };
    }

    Vector3 InternalCamera::GetRotationEuler() const
    {
        float ex, ey, ez;
        QuatToEuler(orientation.x, orientation.y, orientation.z, orientation.w,
            ex, ey, ez);
        return { ex, ey, ez };
    }

    void InternalCamera::SetPosition(float x, float y, float z)
    {
        position = { x, y, z };
    }

    Vector3 InternalCamera::GetPosition() const
    {
        return position;
    }

    void InternalCamera::UpdateCamera()
    {
        raylibCamera.position = position;
        raylibCamera.fovy = fov;

        Vector3 forward = GetForward();
        Vector3 up = GetUp();

        raylibCamera.target = Vector3Add(
            raylibCamera.position,
            Vector3Scale(forward, 100.0f)
        );
        raylibCamera.up = up;

        ::UpdateCamera(&raylibCamera, CAMERA_CUSTOM);
    }

    static InternalCamera CameraInstance;

    InternalCamera& GetCameraInstance()
    {
        return CameraInstance;
    }

}