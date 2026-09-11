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
#include "mathUtils.h"
#include "physical.h"

SGL_API void Graphics3D(int width, int height, const char* title)
{
    SGL::GraphicsInit(width, height, title);
    SGL::GetJoltWorldInstance().Init();
}

SGL_API void ShutdownSGL()
{
    SGL::GetJoltWorldInstance().Shutdown();
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

static void DestroyEntityRecursive(int handle)
{
    SGL::Scene& scene = SGL::GetSceneInstance();
    SGL::JoltWorld& jolt = SGL::GetJoltWorldInstance();

    int c = scene.GetFirstChild(handle);

    while (c != SGL::INVALID_HANDLE)
    {
        int next = scene.GetNextSibling(c);
        DestroyEntityRecursive(c);
        c = next;
    }

    jolt.DestroyBody(handle);
}

SGL_API void DestroyEntity(int handle)
{
    DestroyEntityRecursive(handle);
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

SGL_API void UpdateWorld()
{
    SGL::Scene& scene = SGL::GetSceneInstance();
    SGL::JoltWorld& jolt = SGL::GetJoltWorldInstance();

    for (int i = 0; i < scene.GetEntityCount(); i++)
    {
        if (!scene.IsAlive(i)) continue;
        if (!scene.HasComponent<SGL::Physical>(i)) continue;
        if (!scene.HasComponent<SGL::Transform>(i)) continue;

        const SGL::Physical& physical = scene.GetComponent<SGL::Physical>(i);

        if (physical.anchored)
        {
            jolt.SyncFromECS(i, scene.GetWorldTransform(i));
        }
    }

    static double lastTime = ::GetTime();
    double now = ::GetTime();
    float deltaTime = (float)(now - lastTime);
    lastTime = now;

    if (deltaTime <= 0.0f) deltaTime = 1.0f / 60.0f;
    if (deltaTime > 0.1f) deltaTime = 0.1f;

    jolt.Update(deltaTime);

    for (int i = 0; i < scene.GetEntityCount(); i++)
    {
        if (!scene.IsAlive(i)) continue;
        if (!scene.HasComponent<SGL::Physical>(i)) continue;
        if (!scene.HasComponent<SGL::Transform>(i)) continue;

        const SGL::Physical& physical = scene.GetComponent<SGL::Physical>(i);

        if (physical.anchored) continue;

        SGL::Transform world = scene.GetWorldTransform(i);
        jolt.SyncToECS(i, world);
        scene.SetWorldTransform(i, world);
    }
}

SGL_API void RenderWorld()
{
    SGL::GraphicsRender();
}

SGL_API void SetEntityPosition(int handle, float x, float y, float z)
{
    SGL::GetSceneInstance().SetEntityPosition(handle, x, y, z);

    if (SGL::GetSceneInstance().HasComponent<SGL::Physical>(handle))
    {
        SGL::GetJoltWorldInstance().SyncFromECS(handle, SGL::GetSceneInstance().GetWorldTransform(handle));
    }
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

    if (SGL::GetSceneInstance().HasComponent<SGL::Physical>(handle))
    {
        SGL::GetJoltWorldInstance().SyncFromECS(handle, SGL::GetSceneInstance().GetWorldTransform(handle));
    }
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

    if (!SGL::GetSceneInstance().HasComponent<SGL::Physical>(handle)) return;

    SGL::JoltWorld& jolt = SGL::GetJoltWorldInstance();
    SGL::Scene& scene = SGL::GetSceneInstance();

    jolt.DestroyBody(handle);
    jolt.CreateBody(
        handle,
        scene.GetWorldTransform(handle),
        scene.GetComponent<SGL::Physical>(handle),
        scene.HasComponent<SGL::OBB>(handle) ? &scene.GetComponent<SGL::OBB>(handle) : nullptr,
        scene.HasComponent<SGL::Sphere>(handle) ? &scene.GetComponent<SGL::Sphere>(handle) : nullptr,
        scene.HasComponent<SGL::Capsule>(handle) ? &scene.GetComponent<SGL::Capsule>(handle) : nullptr,
        scene.HasComponent<SGL::Cylinder>(handle) ? &scene.GetComponent<SGL::Cylinder>(handle) : nullptr
    );
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

SGL_API void SetEntityAnchored(int handle, int anchored)
{
    if (!SGL::GetSceneInstance().HasComponent<SGL::Physical>(handle)) return;

    SGL::GetSceneInstance().GetComponent<SGL::Physical>(handle).anchored = anchored;

    SGL::GetJoltWorldInstance().SetBodyAnchored(handle, anchored != 0);
}

SGL_API SGL_BOOL GetEntityAnchored(int handle)
{
    if (!SGL::GetSceneInstance().HasComponent<SGL::Physical>(handle)) return 0;

    return SGL::GetSceneInstance().GetComponent<SGL::Physical>(handle).anchored;
}

SGL_API void SetEntityAllowSleeping(int handle, int allow)
{
    if (!SGL::GetSceneInstance().HasComponent<SGL::Physical>(handle)) return;

    SGL::GetSceneInstance().GetComponent<SGL::Physical>(handle).allowSleeping = allow;
}

SGL_API SGL_BOOL GetEntityAllowSleeping(int handle)
{
    if (!SGL::GetSceneInstance().HasComponent<SGL::Physical>(handle)) return 0;

    return SGL::GetSceneInstance().GetComponent<SGL::Physical>(handle).allowSleeping;
}

SGL_API void SetEntitySensor(int handle, int sensor)
{
    if (!SGL::GetSceneInstance().HasComponent<SGL::Physical>(handle)) return;

    SGL::GetSceneInstance().GetComponent<SGL::Physical>(handle).isSensor = sensor;
}

SGL_API SGL_BOOL GetEntitySensor(int handle)
{
    if (!SGL::GetSceneInstance().HasComponent<SGL::Physical>(handle)) return 0;

    return SGL::GetSceneInstance().GetComponent<SGL::Physical>(handle).isSensor;
}

SGL_API void SetEntityFriction(int handle, float friction)
{
    if (!SGL::GetSceneInstance().HasComponent<SGL::Physical>(handle)) return;

    SGL::GetSceneInstance().GetComponent<SGL::Physical>(handle).friction = friction;
}

SGL_API float GetEntityFriction(int handle)
{
    if (!SGL::GetSceneInstance().HasComponent<SGL::Physical>(handle)) return 0.0f;

    return SGL::GetSceneInstance().GetComponent<SGL::Physical>(handle).friction;
}

SGL_API void SetEntityRestitution(int handle, float restitution)
{
    if (!SGL::GetSceneInstance().HasComponent<SGL::Physical>(handle)) return;

    SGL::GetSceneInstance().GetComponent<SGL::Physical>(handle).restitution = restitution;
}

SGL_API float GetEntityRestitution(int handle)
{
    if (!SGL::GetSceneInstance().HasComponent<SGL::Physical>(handle)) return 0.0f;

    return SGL::GetSceneInstance().GetComponent<SGL::Physical>(handle).restitution;
}

SGL_API void SetEntityLinearDamping(int handle, float damping)
{
    if (!SGL::GetSceneInstance().HasComponent<SGL::Physical>(handle)) return;

    SGL::GetSceneInstance().GetComponent<SGL::Physical>(handle).linearDamping = damping;
}

SGL_API float GetEntityLinearDamping(int handle)
{
    if (!SGL::GetSceneInstance().HasComponent<SGL::Physical>(handle)) return 0.0f;

    return SGL::GetSceneInstance().GetComponent<SGL::Physical>(handle).linearDamping;
}

SGL_API void SetEntityAngularDamping(int handle, float damping)
{
    if (!SGL::GetSceneInstance().HasComponent<SGL::Physical>(handle)) return;

    SGL::GetSceneInstance().GetComponent<SGL::Physical>(handle).angularDamping = damping;
}

SGL_API float GetEntityAngularDamping(int handle)
{
    if (!SGL::GetSceneInstance().HasComponent<SGL::Physical>(handle)) return 0.0f;

    return SGL::GetSceneInstance().GetComponent<SGL::Physical>(handle).angularDamping;
}

SGL_API void SetEntityGravityFactor(int handle, float factor)
{
    if (!SGL::GetSceneInstance().HasComponent<SGL::Physical>(handle)) return;

    SGL::GetSceneInstance().GetComponent<SGL::Physical>(handle).gravityFactor = factor;
}

SGL_API float GetEntityGravityFactor(int handle)
{
    if (!SGL::GetSceneInstance().HasComponent<SGL::Physical>(handle)) return 0.0f;

    return SGL::GetSceneInstance().GetComponent<SGL::Physical>(handle).gravityFactor;
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

SGL_API void SetEntityVelocity(int handle, float vx, float vy, float vz)
{
    SGL::GetJoltWorldInstance().SetBodyVelocity(handle, vx, vy, vz);
}

SGL_API void GetEntityVelocity(int handle, float* vx, float* vy, float* vz)
{
    SGL::GetJoltWorldInstance().GetBodyVelocity(handle, vx, vy, vz);
}

SGL_API void ApplyEntityImpulse(int handle, float fx, float fy, float fz)
{
    SGL::GetJoltWorldInstance().ApplyBodyImpulse(handle, fx, fy, fz);
}

SGL_API void SetEntityAngularVelocity(int handle, float wx, float wy, float wz)
{
    SGL::GetJoltWorldInstance().SetBodyAngularVelocity(handle, wx, wy, wz);
}

SGL_API void GetEntityAngularVelocity(int handle, float* wx, float* wy, float* wz)
{
    SGL::GetJoltWorldInstance().GetBodyAngularVelocity(handle, wx, wy, wz);
}

SGL_API void ApplyEntityAngularImpulse(int handle, float wx, float wy, float wz)
{
    SGL::GetJoltWorldInstance().ApplyBodyAngularImpulse(handle, wx, wy, wz);
}

SGL_API SGL_RayHit Raycast(SGL_Ray ray)
{
    return SGL::PhysicalRaycast(ray);
}

SGL_API double GetElapsedTime()
{
    return ::GetTime();
}