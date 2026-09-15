#include "SGL.h"
#include "scene.h"
#include "component.h"
#include "camera.h"
#include "graphics.h"
#include "cube.h"
#include "sphere.h"
#include "capsule.h"
#include "cylinder.h"
#include "meshEntity.h"
#include "directionalLight.h"
#include "pointLight.h"
#include "spotLight.h"
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
    SGL::GraphicsShutdown();
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

SGL_API void SetCameraNearFarPlane(float nearPlane, float farPlane)
{
    SGL::GetCameraInstance().SetNearFarPlane(nearPlane, farPlane);
}

SGL_API void GetCameraNearFarPlane(float* nearPlane, float* farPlane)
{
    SGL::GetCameraInstance().GetNearFarPlane(nearPlane, farPlane);
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

SGL_API void SetGroundAmbient(float r, float g, float b)
{
    SGL::GraphicsSetGroundAmbient(r, g, b);
}

SGL_API void GetGroundAmbient(float* r, float* g, float* b)
{
    SGL::GraphicsGetGroundAmbient(r, g, b);
}

SGL_API void SetSkyAmbient(float r, float g, float b)
{
    SGL::GraphicsSetSkyAmbient(r, g, b);
}

SGL_API void GetSkyAmbient(float* r, float* g, float* b)
{
    SGL::GraphicsGetSkyAmbient(r, g, b);
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

SGL_API SGL_ENTITY CreateMesh(SGL_MESH meshHandle)
{
    return SGL::CreateMesh(meshHandle);
}

SGL_API SGL_ENTITY CreateDirectionalLight()
{
    return SGL::CreateDirectionalLight();
}

SGL_API SGL_ENTITY CreatePointLight()
{
    return SGL::CreatePointLight();
}

SGL_API SGL_ENTITY CreateSpotLight()
{
    return SGL::CreateSpotLight();
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

SGL_API void SetLightIntensity(SGL_ENTITY handle, float intensity)
{
    SGL::Scene& scene = SGL::GetSceneInstance();

    if (scene.HasComponent<SGL::DirectionalLight>(handle))
    {
        scene.GetComponent<SGL::DirectionalLight>(handle).intensity = intensity;
        return;
    }

    if (scene.HasComponent<SGL::PointLight>(handle))
    {
        scene.GetComponent<SGL::PointLight>(handle).intensity = intensity;
        return;
    }

    if (scene.HasComponent<SGL::SpotLight>(handle))
    {
        scene.GetComponent<SGL::SpotLight>(handle).intensity = intensity;
        return;
    }
}

SGL_API float GetLightIntensity(SGL_ENTITY handle)
{
    SGL::Scene& scene = SGL::GetSceneInstance();

    if (scene.HasComponent<SGL::DirectionalLight>(handle))
        return scene.GetComponent<SGL::DirectionalLight>(handle).intensity;

    if (scene.HasComponent<SGL::PointLight>(handle))
        return scene.GetComponent<SGL::PointLight>(handle).intensity;

    if (scene.HasComponent<SGL::SpotLight>(handle))
        return scene.GetComponent<SGL::SpotLight>(handle).intensity;

    return 0.0f;
}

SGL_API void SetLightShadow(SGL_ENTITY handle, SGL_BOOL shadow)
{
    SGL::Scene& scene = SGL::GetSceneInstance();

    if (scene.HasComponent<SGL::DirectionalLight>(handle))
    {
        scene.GetComponent<SGL::DirectionalLight>(handle).shadow = shadow;
        return;
    }

    if (scene.HasComponent<SGL::PointLight>(handle))
    {
        scene.GetComponent<SGL::PointLight>(handle).shadow = shadow;
        return;
    }

    if (scene.HasComponent<SGL::SpotLight>(handle))
    {
        scene.GetComponent<SGL::SpotLight>(handle).shadow = shadow;
        return;
    }
}

SGL_API SGL_BOOL GetLightShadow(SGL_ENTITY handle)
{
    SGL::Scene& scene = SGL::GetSceneInstance();

    if (scene.HasComponent<SGL::DirectionalLight>(handle))
        return scene.GetComponent<SGL::DirectionalLight>(handle).shadow;

    if (scene.HasComponent<SGL::PointLight>(handle))
        return scene.GetComponent<SGL::PointLight>(handle).shadow;

    if (scene.HasComponent<SGL::SpotLight>(handle))
        return scene.GetComponent<SGL::SpotLight>(handle).shadow;

    return 0;
}

SGL_API void SetLightSpecular(SGL_ENTITY handle, SGL_BOOL specular)
{
    SGL::Scene& scene = SGL::GetSceneInstance();

    if (scene.HasComponent<SGL::DirectionalLight>(handle))
    {
        scene.GetComponent<SGL::DirectionalLight>(handle).specular = specular;
        return;
    }

    if (scene.HasComponent<SGL::PointLight>(handle))
    {
        scene.GetComponent<SGL::PointLight>(handle).specular = specular;
        return;
    }

    if (scene.HasComponent<SGL::SpotLight>(handle))
    {
        scene.GetComponent<SGL::SpotLight>(handle).specular = specular;
        return;
    }
}

SGL_API SGL_BOOL GetLightSpecular(SGL_ENTITY handle)
{
    SGL::Scene& scene = SGL::GetSceneInstance();

    if (scene.HasComponent<SGL::DirectionalLight>(handle))
        return scene.GetComponent<SGL::DirectionalLight>(handle).specular;

    if (scene.HasComponent<SGL::PointLight>(handle))
        return scene.GetComponent<SGL::PointLight>(handle).specular;

    if (scene.HasComponent<SGL::SpotLight>(handle))
        return scene.GetComponent<SGL::SpotLight>(handle).specular;

    return 0;
}

SGL_API void SetLightRange(SGL_ENTITY handle, float range)
{
    SGL::Scene& scene = SGL::GetSceneInstance();

    if (scene.HasComponent<SGL::PointLight>(handle))
    {
        scene.GetComponent<SGL::PointLight>(handle).range = range;
        return;
    }

    if (scene.HasComponent<SGL::SpotLight>(handle))
    {
        scene.GetComponent<SGL::SpotLight>(handle).range = range;
        return;
    }
}

SGL_API float GetLightRange(SGL_ENTITY handle)
{
    SGL::Scene& scene = SGL::GetSceneInstance();

    if (scene.HasComponent<SGL::PointLight>(handle))
        return scene.GetComponent<SGL::PointLight>(handle).range;

    if (scene.HasComponent<SGL::SpotLight>(handle))
        return scene.GetComponent<SGL::SpotLight>(handle).range;

    return 0.0f;
}

SGL_API void SetLightInnerAngle(SGL_ENTITY handle, float innerAngle)
{
    SGL::Scene& scene = SGL::GetSceneInstance();

    if (!scene.HasComponent<SGL::SpotLight>(handle)) return;

    scene.GetComponent<SGL::SpotLight>(handle).innerAngle = innerAngle;
}

SGL_API float GetLightInnerAngle(SGL_ENTITY handle)
{
    SGL::Scene& scene = SGL::GetSceneInstance();

    if (!scene.HasComponent<SGL::SpotLight>(handle)) return 0.0f;

    return scene.GetComponent<SGL::SpotLight>(handle).innerAngle;
}

SGL_API void SetLightOuterAngle(SGL_ENTITY handle, float outerAngle)
{
    SGL::Scene& scene = SGL::GetSceneInstance();

    if (!scene.HasComponent<SGL::SpotLight>(handle)) return;

    scene.GetComponent<SGL::SpotLight>(handle).outerAngle = outerAngle;
}

SGL_API float GetLightOuterAngle(SGL_ENTITY handle)
{
    SGL::Scene& scene = SGL::GetSceneInstance();

    if (!scene.HasComponent<SGL::SpotLight>(handle)) return 0.0f;

    return scene.GetComponent<SGL::SpotLight>(handle).outerAngle;
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

SGL_API void SetGravity(float x, float y, float z)
{
    SGL::GetJoltWorldInstance().SetGravity(x, y, z);
}

SGL_API void GetGravity(float* x, float* y, float* z)
{
    SGL::GetJoltWorldInstance().GetGravity(x, y, z);
}

SGL_API SGL_MESH LoadMeshAsset(const char* fileName)
{
    return SGL::LoadMesh(fileName);
}

SGL_API void UnloadMeshAsset(SGL_MESH handle)
{
    SGL::UnloadMesh(handle);
}

SGL_API double GetElapsedTime()
{
    return ::GetTime();
}