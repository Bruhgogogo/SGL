#pragma once

// #define SGL_EXPORT
#define SGL_STATIC

#ifdef SGL_STATIC
#define SGL_API
#else
#ifdef SGL_EXPORT
#define SGL_API __declspec(dllexport)
#else
#define SGL_API __declspec(dllimport)
#endif
#endif

typedef int SGL_ENTITY;
typedef int SGL_BOOL;

typedef SGL_ENTITY SGL_MESH;

SGL_BOOL SGL_TRUE = 1;
SGL_BOOL SGL_FALSE = 0;

typedef enum
{
    SGL_WINDOWED,
    SGL_BORDERLESS,
    SGL_FULLSCREEN
} SGL_WindowMode;

typedef struct
{
    float startX;
    float startY;
    float startZ;
    float endX;
    float endY;
    float endZ;
} SGL_Ray;

typedef struct
{
    SGL_BOOL hit;
    int      entity;
    float    pointX;
    float    pointY;
    float    pointZ;
    float    normalX;
    float    normalY;
    float    normalZ;
    float    distance;
} SGL_RayHit;

#ifdef __cplusplus
extern "C" {
#endif

    SGL_API void Graphics3D(int width, int height, const char* title);
    SGL_API void ShutdownSGL();
    SGL_API SGL_BOOL IsWindowShouldClose();

    SGL_API void GetWindowWidth(int* width);
    SGL_API void GetWindowHeight(int* height);
    SGL_API void GetWindowMode(SGL_WindowMode* mode);

    SGL_API void SetWindowWidth(int width);
    SGL_API void SetWindowHeight(int height);
    SGL_API void SetWindowMode(SGL_WindowMode mode);

    SGL_API void SetCameraPosition(float x, float y, float z);
    SGL_API void GetCameraPosition(float* x, float* y, float* z);
    SGL_API void SetCameraRotation(float pitch, float yaw, float roll);
    SGL_API void GetCameraRotation(float* pitch, float* yaw, float* roll);
    SGL_API void SetCameraFOV(float fov);
    SGL_API void GetCameraFOV(float* fov);

    SGL_API SGL_ENTITY  CreateEntity();
    SGL_API void DestroyEntity(SGL_ENTITY handle);

    SGL_API void SetParent(SGL_ENTITY child, SGL_ENTITY parent);
    SGL_API SGL_ENTITY  GetParent(SGL_ENTITY handle);

    SGL_API void UpdateWorld();
    SGL_API void RenderWorld();

    SGL_API void SetEntityPosition(SGL_ENTITY handle, float x, float y, float z);
    SGL_API void GetEntityPosition(SGL_ENTITY handle, float* x, float* y, float* z);
    SGL_API void GetEntityLocalPosition(SGL_ENTITY handle, float* x, float* y, float* z);

    SGL_API void SetEntityRotation(SGL_ENTITY handle, float pitch, float yaw, float roll);
    SGL_API void GetEntityRotation(SGL_ENTITY handle, float* pitch, float* yaw, float* roll);
    SGL_API void GetEntityLocalRotation(SGL_ENTITY handle, float* pitch, float* yaw, float* roll);

    SGL_API void SetEntityScale(SGL_ENTITY handle, float sx, float sy, float sz);
    SGL_API void GetEntityScale(SGL_ENTITY handle, float* sx, float* sy, float* sz);
    SGL_API void GetEntityLocalScale(SGL_ENTITY handle, float* sx, float* sy, float* sz);

    SGL_API SGL_ENTITY  CreateCube();
    SGL_API SGL_ENTITY  CreateSphere();
    SGL_API SGL_ENTITY  CreateCapsule();
    SGL_API SGL_ENTITY  CreateCylinder();

    SGL_API void SetEntityColor(SGL_ENTITY handle, unsigned char r, unsigned char g, unsigned char b, unsigned char a);
    SGL_API void GetEntityColor(SGL_ENTITY handle, unsigned char* r, unsigned char* g, unsigned char* b, unsigned char* a);

    SGL_API void SetEntityVisible(SGL_ENTITY handle, SGL_BOOL visible);
    SGL_API SGL_BOOL  GetEntityVisible(SGL_ENTITY handle);

    SGL_API void SetEntityMeshID(SGL_ENTITY handle, SGL_MESH meshID);
    SGL_API SGL_MESH  GetEntityMeshID(SGL_ENTITY handle);

    SGL_API SGL_BOOL IsEntityValid(SGL_ENTITY handle);

    SGL_API void SetEntityCollide(SGL_ENTITY handle, SGL_BOOL canCollide);
    SGL_API SGL_BOOL  GetEntityCollide(SGL_ENTITY handle);

    SGL_API void SetEntityAnchored(SGL_ENTITY handle, SGL_BOOL anchored);
    SGL_API SGL_BOOL GetEntityAnchored(SGL_ENTITY handle);

    SGL_API void SetEntityAllowSleeping(SGL_ENTITY handle, SGL_BOOL allow);
    SGL_API SGL_BOOL GetEntityAllowSleeping(SGL_ENTITY handle);

    SGL_API void SetEntitySensor(SGL_ENTITY handle, SGL_BOOL sensor);
    SGL_API SGL_BOOL GetEntitySensor(SGL_ENTITY handle);

    SGL_API void SetEntityFriction(SGL_ENTITY handle, float friction);
    SGL_API float GetEntityFriction(SGL_ENTITY handle);

    SGL_API void SetEntityRestitution(SGL_ENTITY handle, float restitution);
    SGL_API float GetEntityRestitution(SGL_ENTITY handle);

    SGL_API void SetEntityLinearDamping(SGL_ENTITY handle, float damping);
    SGL_API float GetEntityLinearDamping(SGL_ENTITY handle);

    SGL_API void SetEntityAngularDamping(SGL_ENTITY handle, float damping);
    SGL_API float GetEntityAngularDamping(SGL_ENTITY handle);

    SGL_API void SetEntityGravityFactor(SGL_ENTITY handle, float factor);
    SGL_API float GetEntityGravityFactor(SGL_ENTITY handle);

    SGL_API void SetEntityMass(SGL_ENTITY handle, float mass);
    SGL_API float GetEntityMass(SGL_ENTITY handle);

    SGL_API void SetEntityVelocity(SGL_ENTITY handle, float vx, float vy, float vz);
    SGL_API void GetEntityVelocity(SGL_ENTITY handle, float* vx, float* vy, float* vz);

    SGL_API void ApplyEntityImpulse(SGL_ENTITY handle, float fx, float fy, float fz);

    SGL_API void SetEntityAngularVelocity(SGL_ENTITY handle, float wx, float wy, float wz);
    SGL_API void GetEntityAngularVelocity(SGL_ENTITY handle, float* wx, float* wy, float* wz);

    SGL_API void ApplyEntityAngularImpulse(SGL_ENTITY handle, float wx, float wy, float wz);

    SGL_API SGL_RayHit Raycast(SGL_Ray ray);

    SGL_API double GetElapsedTime();

#ifdef __cplusplus
}
#endif