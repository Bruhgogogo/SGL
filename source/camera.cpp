#include "SGL.h"
#include "camera.h"

namespace SGL
{

    raylib::Vector3 InternalCamera::GetForward() const
    {
        return raylib::Vector3RotateByQuaternion({ 0.0f, 0.0f, -1.0f }, orientation);
    }

    raylib::Vector3 InternalCamera::GetUp() const
    {
        return raylib::Vector3RotateByQuaternion({ 0.0f, 1.0f, 0.0f }, orientation);
    }

    void InternalCamera::SetRotationEuler(raylib::Vector3 deg)
    {
        float qx, qy, qz, qw;
        EulerToQuat(deg.x, deg.y, deg.z, qx, qy, qz, qw);
        orientation = { qx, qy, qz, qw };
    }

    raylib::Vector3 InternalCamera::GetRotationEuler() const
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

    raylib::Vector3 InternalCamera::GetPosition() const
    {
        return position;
    }

    void InternalCamera::UpdateCamera()
    {
        raylibCamera.position = position;
        raylibCamera.fovy = fov;

        raylib::Vector3 forward = GetForward();
        raylib::Vector3 up = GetUp();

        raylibCamera.target = raylib::Vector3Add(
            raylibCamera.position,
            raylib::Vector3Scale(forward, 100.0f)
        );
        raylibCamera.up = up;

        raylib::UpdateCamera(&raylibCamera, raylib::CAMERA_CUSTOM);
    }

    static InternalCamera CameraInstance;

    InternalCamera& GetCameraInstance()
    {
        return CameraInstance;
    }

}

void SetCameraPosition(float x, float y, float z)
{
    SGL::GetCameraInstance().SetPosition(x, y, z);
}

void GetCameraPosition(float* x, float* y, float* z)
{
    raylib::Vector3 position = SGL::GetCameraInstance().GetPosition();

    *x = position.x;
    *y = position.y;
    *z = position.z;
}

void SetCameraRotation(float pitch, float yaw, float roll)
{
    SGL::GetCameraInstance().SetRotationEuler(raylib::Vector3(pitch, yaw, roll));
}

void GetCameraRotation(float* pitch, float* yaw, float* roll)
{
    raylib::Vector3 rotation = SGL::GetCameraInstance().GetRotationEuler();

    *pitch = rotation.x;
    *yaw = rotation.y;
    *roll = rotation.z;
}

void SetCameraFOV(float fov)
{
    SGL::GetCameraInstance().fov = fov;
}

void GetCameraFOV(float* fov)
{
    *fov = SGL::GetCameraInstance().fov;
}