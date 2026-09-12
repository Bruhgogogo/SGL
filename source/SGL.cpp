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

SGL_API SGL_ENTITY CreateEntity()
{
    return SGL::GetSceneInstance().CreateEntity();
}

static void DestroyEntityRecursive(SGL_ENTITY handle)
{
    SGL::Scene& scene = SGL::GetSceneInstance();
    SGL::JoltWorld& jolt = SGL::GetJoltWorldInstance();

    SGL_ENTITY c = scene.GetFirstChild(handle);

    while (c != SGL::INVALID_HANDLE)
    {
        SGL_ENTITY next = scene.GetNextSibling(c);
        DestroyEntityRecursive(c);
        c = next;
    }

    jolt.DestroyBody(handle);
}

SGL_API void DestroyEntity(SGL_ENTITY handle)
{
    DestroyEntityRecursive(handle);
    SGL::GetSceneInstance().DestroyEntity(handle);
}

SGL_API void SetParent(SGL_ENTITY child, SGL_ENTITY parent)
{
    SGL::GetSceneInstance().SetParent(child, parent);
}

SGL_API SGL_ENTITY GetParent(SGL_ENTITY handle)
{
    return SGL::GetSceneInstance().GetParent(handle);
}

SGL_API void UpdateWorld(int collisionSteps)
{
    static double lastTime = GetElapsedTime();
    double now = GetElapsedTime();
    float deltaTime = (float)(now - lastTime);
    lastTime = now;

    if (deltaTime <= 0.0f) deltaTime = 1.0f / 60.0f;
    if (deltaTime > 0.1f) deltaTime = 0.1f;

    SGL::GetJoltWorldInstance().PhysicalUpdateWorld(deltaTime, collisionSteps);
}

SGL_API void RenderWorld()
{
    SGL::GraphicsRender();
}

SGL_API void SetEntityPosition(SGL_ENTITY handle, float x, float y, float z)
{
    SGL::GetSceneInstance().SetEntityPosition(handle, x, y, z);

    if (SGL::GetSceneInstance().HasComponent<SGL::Physical>(handle))
    {
        SGL::GetJoltWorldInstance().SyncFromECS(handle, SGL::GetSceneInstance().GetWorldTransform(handle));
    }
}

SGL_API void GetEntityPosition(SGL_ENTITY handle, float* x, float* y, float* z)
{
    SGL::GetSceneInstance().GetEntityPosition(handle, x, y, z);
}

SGL_API void GetEntityLocalPosition(SGL_ENTITY handle, float* x, float* y, float* z)
{
    SGL::GetSceneInstance().GetEntityLocalPosition(handle, x, y, z);
}

SGL_API void SetEntityRotation(SGL_ENTITY handle, float pitch, float yaw, float roll)
{
    SGL::GetSceneInstance().SetEntityRotation(handle, pitch, yaw, roll);

    if (SGL::GetSceneInstance().HasComponent<SGL::Physical>(handle))
    {
        SGL::GetJoltWorldInstance().SyncFromECS(handle, SGL::GetSceneInstance().GetWorldTransform(handle));
    }
}

SGL_API void GetEntityRotation(SGL_ENTITY handle, float* pitch, float* yaw, float* roll)
{
    SGL::GetSceneInstance().GetEntityRotation(handle, pitch, yaw, roll);
}

SGL_API void GetEntityLocalRotation(SGL_ENTITY handle, float* pitch, float* yaw, float* roll)
{
    SGL::GetSceneInstance().GetEntityLocalRotation(handle, pitch, yaw, roll);
}

SGL_API void SetEntityScale(SGL_ENTITY handle, float sx, float sy, float sz)
{
    SGL::GetSceneInstance().SetEntityScale(handle, sx, sy, sz);
    SGL::GetJoltWorldInstance().PhysicalRebuildBody(handle);
}

SGL_API void GetEntityScale(SGL_ENTITY handle, float* sx, float* sy, float* sz)
{
    SGL::GetSceneInstance().GetEntityScale(handle, sx, sy, sz);
}

SGL_API void GetEntityLocalScale(SGL_ENTITY handle, float* sx, float* sy, float* sz)
{
    SGL::GetSceneInstance().GetEntityLocalScale(handle, sx, sy, sz);
}

SGL_API SGL_ENTITY CreateCube()
{
    return SGL::CreateCube();
}

SGL_API SGL_ENTITY CreateSphere()
{
    return SGL::CreateSphere();
}

SGL_API SGL_ENTITY CreateCapsule()
{
    return SGL::CreateCapsule();
}

SGL_API SGL_ENTITY CreateCylinder()
{
    return SGL::CreateCylinder();
}

SGL_API void SetEntityColor(SGL_ENTITY handle, unsigned char r, unsigned char g, unsigned char b, unsigned char a)
{
    if (!SGL::GetSceneInstance().HasComponent<SGL::Color>(handle)) return;

    SGL::Color& c = SGL::GetSceneInstance().GetComponent<SGL::Color>(handle);

    c.r = r;
    c.g = g;
    c.b = b;
    c.a = a;
}

SGL_API void GetEntityColor(SGL_ENTITY handle, unsigned char* r, unsigned char* g, unsigned char* b, unsigned char* a)
{
    if (!SGL::GetSceneInstance().HasComponent<SGL::Color>(handle)) return;

    SGL::Color& c = SGL::GetSceneInstance().GetComponent<SGL::Color>(handle);

    *r = c.r;
    *g = c.g;
    *b = c.b;
    *a = c.a;
}

SGL_API void SetEntityVisible(SGL_ENTITY handle, SGL_BOOL visible)
{
    if (!SGL::GetSceneInstance().HasComponent<SGL::Visible>(handle)) return;

    SGL::GetSceneInstance().GetComponent<SGL::Visible>(handle).value = visible;
}

SGL_API SGL_BOOL GetEntityVisible(SGL_ENTITY handle)
{
    if (!SGL::GetSceneInstance().HasComponent<SGL::Visible>(handle)) return 0;

    return SGL::GetSceneInstance().GetComponent<SGL::Visible>(handle).value;
}

SGL_API void SetEntityMeshID(SGL_ENTITY handle, SGL_MESH meshID)
{
    SGL::GetSceneInstance().SetEntityMeshID(handle, meshID);
}

SGL_API SGL_MESH GetEntityMeshID(SGL_ENTITY handle)
{
    if (!SGL::GetSceneInstance().HasComponent<SGL::MeshID>(handle)) return -1;

    return SGL::GetSceneInstance().GetComponent<SGL::MeshID>(handle).value;
}

SGL_API SGL_BOOL IsEntityValid(SGL_ENTITY handle)
{
    return SGL::GetSceneInstance().IsAlive(handle) ? 1 : 0;
}

SGL_API void SetEntityCollide(SGL_ENTITY handle, SGL_BOOL canCollide)
{
    SGL::Scene& scene = SGL::GetSceneInstance();

    if (!scene.HasComponent<SGL::Physical>(handle)) return;

    SGL::Physical& physical = scene.GetComponent<SGL::Physical>(handle);

    physical.canCollide = canCollide;

    SGL::GetJoltWorldInstance().SetBodyCollide(handle, canCollide != 0);
}

SGL_API SGL_BOOL GetEntityCollide(SGL_ENTITY handle)
{
    if (!SGL::GetSceneInstance().HasComponent<SGL::Physical>(handle)) return 0;

    return SGL::GetSceneInstance().GetComponent<SGL::Physical>(handle).canCollide;
}

SGL_API void SetEntityAnchored(SGL_ENTITY handle, SGL_BOOL anchored)
{
    SGL::Scene& scene = SGL::GetSceneInstance();

    if (!scene.HasComponent<SGL::Physical>(handle)) return;

    SGL::Physical& physical = scene.GetComponent<SGL::Physical>(handle);

    physical.anchored = anchored;

    SGL::GetJoltWorldInstance().SetBodyAnchored(handle, anchored != 0);
}

SGL_API SGL_BOOL GetEntityAnchored(SGL_ENTITY handle)
{
    if (!SGL::GetSceneInstance().HasComponent<SGL::Physical>(handle)) return 0;

    return SGL::GetSceneInstance().GetComponent<SGL::Physical>(handle).anchored;
}

SGL_API void SetEntityAllowSleeping(SGL_ENTITY handle, SGL_BOOL allow)
{
    if (!SGL::GetSceneInstance().HasComponent<SGL::Physical>(handle)) return;

    SGL::GetSceneInstance().GetComponent<SGL::Physical>(handle).allowSleeping = allow;
}

SGL_API SGL_BOOL GetEntityAllowSleeping(SGL_ENTITY handle)
{
    if (!SGL::GetSceneInstance().HasComponent<SGL::Physical>(handle)) return 0;

    return SGL::GetSceneInstance().GetComponent<SGL::Physical>(handle).allowSleeping;
}

SGL_API void SetEntitySensor(SGL_ENTITY handle, SGL_BOOL sensor)
{
    if (!SGL::GetSceneInstance().HasComponent<SGL::Physical>(handle)) return;

    SGL::GetSceneInstance().GetComponent<SGL::Physical>(handle).isSensor = sensor;
}

SGL_API SGL_BOOL GetEntitySensor(SGL_ENTITY handle)
{
    if (!SGL::GetSceneInstance().HasComponent<SGL::Physical>(handle)) return 0;

    return SGL::GetSceneInstance().GetComponent<SGL::Physical>(handle).isSensor;
}

SGL_API void SetEntityFriction(SGL_ENTITY handle, float friction)
{
    if (!SGL::GetSceneInstance().HasComponent<SGL::Physical>(handle)) return;

    SGL::GetSceneInstance().GetComponent<SGL::Physical>(handle).friction = friction;
}

SGL_API float GetEntityFriction(SGL_ENTITY handle)
{
    if (!SGL::GetSceneInstance().HasComponent<SGL::Physical>(handle)) return 0.0f;

    return SGL::GetSceneInstance().GetComponent<SGL::Physical>(handle).friction;
}

SGL_API void SetEntityRestitution(SGL_ENTITY handle, float restitution)
{
    if (!SGL::GetSceneInstance().HasComponent<SGL::Physical>(handle)) return;

    SGL::GetSceneInstance().GetComponent<SGL::Physical>(handle).restitution = restitution;
}

SGL_API float GetEntityRestitution(SGL_ENTITY handle)
{
    if (!SGL::GetSceneInstance().HasComponent<SGL::Physical>(handle)) return 0.0f;

    return SGL::GetSceneInstance().GetComponent<SGL::Physical>(handle).restitution;
}

SGL_API void SetEntityLinearDamping(SGL_ENTITY handle, float damping)
{
    if (!SGL::GetSceneInstance().HasComponent<SGL::Physical>(handle)) return;

    SGL::GetSceneInstance().GetComponent<SGL::Physical>(handle).linearDamping = damping;
}

SGL_API float GetEntityLinearDamping(SGL_ENTITY handle)
{
    if (!SGL::GetSceneInstance().HasComponent<SGL::Physical>(handle)) return 0.0f;

    return SGL::GetSceneInstance().GetComponent<SGL::Physical>(handle).linearDamping;
}

SGL_API void SetEntityAngularDamping(SGL_ENTITY handle, float damping)
{
    if (!SGL::GetSceneInstance().HasComponent<SGL::Physical>(handle)) return;

    SGL::GetSceneInstance().GetComponent<SGL::Physical>(handle).angularDamping = damping;
}

SGL_API float GetEntityAngularDamping(SGL_ENTITY handle)
{
    if (!SGL::GetSceneInstance().HasComponent<SGL::Physical>(handle)) return 0.0f;

    return SGL::GetSceneInstance().GetComponent<SGL::Physical>(handle).angularDamping;
}

SGL_API void SetEntityGravityFactor(SGL_ENTITY handle, float factor)
{
    if (!SGL::GetSceneInstance().HasComponent<SGL::Physical>(handle)) return;

    SGL::GetSceneInstance().GetComponent<SGL::Physical>(handle).gravityFactor = factor;
}

SGL_API float GetEntityGravityFactor(SGL_ENTITY handle)
{
    if (!SGL::GetSceneInstance().HasComponent<SGL::Physical>(handle)) return 0.0f;

    return SGL::GetSceneInstance().GetComponent<SGL::Physical>(handle).gravityFactor;
}

SGL_API void SetEntityMass(SGL_ENTITY handle, float mass)
{
    if (!SGL::GetSceneInstance().HasComponent<SGL::Physical>(handle)) return;

    SGL::GetSceneInstance().GetComponent<SGL::Physical>(handle).mass = mass;
}

SGL_API float GetEntityMass(SGL_ENTITY handle)
{
    if (!SGL::GetSceneInstance().HasComponent<SGL::Physical>(handle)) return 0.0f;

    return SGL::GetSceneInstance().GetComponent<SGL::Physical>(handle).mass;
}

SGL_API void SetEntityVelocity(SGL_ENTITY handle, float vx, float vy, float vz)
{
    SGL::GetJoltWorldInstance().SetBodyVelocity(handle, vx, vy, vz);
}

SGL_API void GetEntityVelocity(SGL_ENTITY handle, float* vx, float* vy, float* vz)
{
    SGL::GetJoltWorldInstance().GetBodyVelocity(handle, vx, vy, vz);
}

SGL_API void ApplyEntityImpulse(SGL_ENTITY handle, float fx, float fy, float fz)
{
    SGL::GetJoltWorldInstance().ApplyBodyImpulse(handle, fx, fy, fz);
}

SGL_API void SetEntityAngularVelocity(SGL_ENTITY handle, float wx, float wy, float wz)
{
    SGL::GetJoltWorldInstance().SetBodyAngularVelocity(handle, wx, wy, wz);
}

SGL_API void GetEntityAngularVelocity(SGL_ENTITY handle, float* wx, float* wy, float* wz)
{
    SGL::GetJoltWorldInstance().GetBodyAngularVelocity(handle, wx, wy, wz);
}

SGL_API void ApplyEntityAngularImpulse(SGL_ENTITY handle, float wx, float wy, float wz)
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