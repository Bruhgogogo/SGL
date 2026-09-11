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

typedef int SGL_BOOL;

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

    SGL_API int  CreateEntity();
    SGL_API void DestroyEntity(int handle);

    SGL_API void SetParent(int child, int parent);
    SGL_API int  GetParent(int handle);

    SGL_API void UpdateWorld();
    SGL_API void RenderWorld();

    SGL_API void SetEntityPosition(int handle, float x, float y, float z);
    SGL_API void GetEntityPosition(int handle, float* x, float* y, float* z);
    SGL_API void GetEntityLocalPosition(int handle, float* x, float* y, float* z);

    SGL_API void SetEntityRotation(int handle, float pitch, float yaw, float roll);
    SGL_API void GetEntityRotation(int handle, float* pitch, float* yaw, float* roll);
    SGL_API void GetEntityLocalRotation(int handle, float* pitch, float* yaw, float* roll);

    SGL_API void SetEntityScale(int handle, float sx, float sy, float sz);
    SGL_API void GetEntityScale(int handle, float* sx, float* sy, float* sz);
    SGL_API void GetEntityLocalScale(int handle, float* sx, float* sy, float* sz);

    SGL_API int  CreateCube();
    SGL_API int  CreateSphere();
    SGL_API int  CreateCapsule();
    SGL_API int  CreateCylinder();

    SGL_API void SetEntityColor(int handle, unsigned char r, unsigned char g, unsigned char b, unsigned char a);
    SGL_API void GetEntityColor(int handle, unsigned char* r, unsigned char* g, unsigned char* b, unsigned char* a);

    SGL_API void SetEntityVisible(int handle, int visible);
    SGL_API SGL_BOOL  GetEntityVisible(int handle);

    SGL_API void SetEntityMeshID(int handle, int meshID);
    SGL_API int  GetEntityMeshID(int handle);

    SGL_API SGL_BOOL IsEntityValid(int handle);

    SGL_API void SetEntityCollide(int handle, int canCollide);
    SGL_API SGL_BOOL  GetEntityCollide(int handle);

    SGL_API void SetEntityAnchored(int handle, int anchored);
    SGL_API SGL_BOOL GetEntityAnchored(int handle);

    SGL_API void SetEntityAllowSleeping(int handle, int allow);
    SGL_API SGL_BOOL GetEntityAllowSleeping(int handle);

    SGL_API void SetEntitySensor(int handle, int sensor);
    SGL_API SGL_BOOL GetEntitySensor(int handle);

    SGL_API void SetEntityFriction(int handle, float friction);
    SGL_API float GetEntityFriction(int handle);

    SGL_API void SetEntityRestitution(int handle, float restitution);
    SGL_API float GetEntityRestitution(int handle);

    SGL_API void SetEntityLinearDamping(int handle, float damping);
    SGL_API float GetEntityLinearDamping(int handle);

    SGL_API void SetEntityAngularDamping(int handle, float damping);
    SGL_API float GetEntityAngularDamping(int handle);

    SGL_API void SetEntityGravityFactor(int handle, float factor);
    SGL_API float GetEntityGravityFactor(int handle);

    SGL_API void SetEntityMass(int handle, float mass);
    SGL_API float GetEntityMass(int handle);

    SGL_API void SetEntityVelocity(int handle, float vx, float vy, float vz);
    SGL_API void GetEntityVelocity(int handle, float* vx, float* vy, float* vz);

    SGL_API void ApplyEntityImpulse(int handle, float fx, float fy, float fz);

    SGL_API void SetEntityAngularVelocity(int handle, float wx, float wy, float wz);
    SGL_API void GetEntityAngularVelocity(int handle, float* wx, float* wy, float* wz);

    SGL_API void ApplyEntityAngularImpulse(int handle, float wx, float wy, float wz);

    SGL_API SGL_RayHit Raycast(SGL_Ray ray);

    SGL_API double GetElapsedTime();

#ifdef __cplusplus
}
#endif