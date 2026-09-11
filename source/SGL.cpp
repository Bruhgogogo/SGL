#include "SGL.h"
#include "scene.h"
#include "component.h"
#include "camera.h"
#include "graphics.h"
#include "cube.h"
#include "sphere.h"
#include "capsule.h"
#include "cylinder.h"
#include "raylibUtils.h"
#include "physical.h"

#include <iostream>

SGL_API void Graphics3D(int width, int height, const char* title)
{
    SGL::GraphicsInit(width, height, title);
}

SGL_API SGL_BOOL IsWindowShouldClose()
{
    return SGL::GraphicsShouldClose() ? 1 : 0;
}

SGL_API void GetWindowWidth(int* width)
{
    SGL::GraphicsGetWindowWidth(width);
}

SGL_API void GetWindowHeight(int* height)
{
    SGL::GraphicsGetWindowHeight(height);
}

SGL_API void SetWindowMode(SGL_WindowMode mode)
{
    SGL::GraphicsSetWindowMode((int)mode);
}

SGL_API void SetCameraPosition(float x, float y, float z)
{
    SGL::GetCameraInstance().SetPosition(x, y, z);
}

SGL_API void GetCameraPosition(float* x, float* y, float* z)
{
    Vector3 position = SGL::GetCameraInstance().GetPosition();

    *x = position.x;
    *y = position.y;
    *z = position.z;
}

SGL_API void SetCameraRotation(float pitch, float yaw, float roll)
{
    SGL::GetCameraInstance().SetRotationEuler(Vector3(pitch, yaw, roll));
}

SGL_API void GetCameraRotation(float* pitch, float* yaw, float* roll)
{
    Vector3 rotation = SGL::GetCameraInstance().GetRotationEuler();

    *pitch = rotation.x;
    *yaw = rotation.y;
    *roll = rotation.z;
}

SGL_API void SetCameraFOV(float fov)
{
    SGL::GetCameraInstance().fov = fov;
}

SGL_API void GetCameraFOV(float* fov)
{
    *fov = SGL::GetCameraInstance().fov;
}

SGL_API int CreateEntity()
{
    return SGL::GetSceneInstance().CreateEntity();
}

SGL_API void DestroyEntity(int handle)
{
    SGL::GetSceneInstance().DestroyEntity(handle);
}

SGL_API void SetParent(int child, int parent)
{
    SGL::GetSceneInstance().SetParent(child, parent);
}

SGL_API int GetParent(int handle)
{
    return SGL::GetSceneInstance().GetParent(handle);
}

SGL_API void UpdateWorld(int handle)
{

}

SGL_API void RenderWorld()
{
    SGL::GraphicsRender();
}

SGL_API void SetEntityPosition(int handle, float x, float y, float z)
{
    SGL::GetSceneInstance().SetEntityPosition(handle, x, y, z);
}

SGL_API void GetEntityPosition(int handle, float* x, float* y, float* z)
{
    SGL::GetSceneInstance().GetEntityPosition(handle, x, y, z);
}

SGL_API void GetEntityLocalPosition(int handle, float* x, float* y, float* z)
{
    SGL::GetSceneInstance().GetEntityLocalPosition(handle, x, y, z);
}

SGL_API void SetEntityRotation(int handle, float pitch, float yaw, float roll)
{
    SGL::GetSceneInstance().SetEntityRotation(handle, pitch, yaw, roll);
}

SGL_API void GetEntityRotation(int handle, float* pitch, float* yaw, float* roll)
{
    SGL::GetSceneInstance().GetEntityRotation(handle, pitch, yaw, roll);
}

SGL_API void GetEntityLocalRotation(int handle, float* pitch, float* yaw, float* roll)
{
    SGL::GetSceneInstance().GetEntityLocalRotation(handle, pitch, yaw, roll);
}

SGL_API void SetEntityScale(int handle, float sx, float sy, float sz)
{
    SGL::GetSceneInstance().SetEntityScale(handle, sx, sy, sz);
}

SGL_API void GetEntityScale(int handle, float* sx, float* sy, float* sz)
{
    SGL::GetSceneInstance().GetEntityScale(handle, sx, sy, sz);
}

SGL_API void GetEntityLocalScale(int handle, float* sx, float* sy, float* sz)
{
    SGL::GetSceneInstance().GetEntityLocalScale(handle, sx, sy, sz);
}

SGL_API int CreateCube()
{
    return SGL::CreateCube();
}

SGL_API int CreateSphere()
{
    return SGL::CreateSphere();
}

SGL_API int CreateCapsule()
{
    return SGL::CreateCapsule();
}

SGL_API int CreateCylinder()
{
    return SGL::CreateCylinder();
}

SGL_API void SetEntityColor(int handle, unsigned char r, unsigned char g, unsigned char b, unsigned char a)
{
    if (!SGL::GetSceneInstance().HasComponent<SGL::Color>(handle)) return;

    SGL::Color& c = SGL::GetSceneInstance().GetComponent<SGL::Color>(handle);

    c.r = r;
    c.g = g;
    c.b = b;
    c.a = a;
}

SGL_API void GetEntityColor(int handle, unsigned char* r, unsigned char* g, unsigned char* b, unsigned char* a)
{
    if (!SGL::GetSceneInstance().HasComponent<SGL::Color>(handle)) return;

    SGL::Color& c = SGL::GetSceneInstance().GetComponent<SGL::Color>(handle);

    *r = c.r;
    *g = c.g;
    *b = c.b;
    *a = c.a;
}

SGL_API void SetEntityVisible(int handle, int visible)
{
    if (!SGL::GetSceneInstance().HasComponent<SGL::Visible>(handle)) return;

    SGL::GetSceneInstance().GetComponent<SGL::Visible>(handle).value = visible;
}

SGL_API SGL_BOOL GetEntityVisible(int handle)
{
    if (!SGL::GetSceneInstance().HasComponent<SGL::Visible>(handle)) return 0;

    return SGL::GetSceneInstance().GetComponent<SGL::Visible>(handle).value;
}

SGL_API void SetEntityMeshID(int handle, int meshID)
{
    SGL::GetSceneInstance().SetEntityMeshID(handle, meshID);
}

SGL_API int GetEntityMeshID(int handle)
{
    if (!SGL::GetSceneInstance().HasComponent<SGL::MeshID>(handle)) return -1;

    return SGL::GetSceneInstance().GetComponent<SGL::MeshID>(handle).value;
}

SGL_API SGL_BOOL IsEntityValid(int handle)
{
    return SGL::GetSceneInstance().IsAlive(handle) ? 1 : 0;
}

SGL_API void SetEntityCollide(int handle, int canCollide)
{
    if (!SGL::GetSceneInstance().HasComponent<SGL::Physical>(handle)) return;

    SGL::GetSceneInstance().GetComponent<SGL::Physical>(handle).canCollide = canCollide;
}

SGL_API SGL_BOOL GetEntityCollide(int handle)
{
    if (!SGL::GetSceneInstance().HasComponent<SGL::Physical>(handle)) return 0;

    return SGL::GetSceneInstance().GetComponent<SGL::Physical>(handle).canCollide;
}

SGL_API void SetEntityMass(int handle, float mass)
{
    if (!SGL::GetSceneInstance().HasComponent<SGL::Physical>(handle)) return;

    SGL::GetSceneInstance().GetComponent<SGL::Physical>(handle).mass = mass;
}

SGL_API float GetEntityMass(int handle)
{
    if (!SGL::GetSceneInstance().HasComponent<SGL::Physical>(handle)) return 0.0f;

    return SGL::GetSceneInstance().GetComponent<SGL::Physical>(handle).mass;
}

SGL_API SGL_BOOL CheckCollision(int a, int b)
{
    return SGL::PhysicalCheckCollision(a, b) ? 1 : 0;
}

SGL_API SGL_RayHit Raycast(SGL_Ray ray)
{
    return SGL::PhysicalRaycast(ray);
}

SGL_API double GetElapsedTime()
{
    return ::GetTime();
}