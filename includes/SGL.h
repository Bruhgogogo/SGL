#pragma once

typedef enum
{
    SGL_WINDOWED,
    SGL_BORDERLESS,
    SGL_FULLSCREEN
} SGL_WindowMode;

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

#ifdef __cplusplus
extern "C" {
#endif

    SGL_API void Graphics3D(int width, int height, const char* title);
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

    SGL_API void SetEntityColor(int handle, unsigned char r, unsigned char g, unsigned char b, unsigned char a);
    SGL_API void GetEntityColor(int handle, unsigned char* r, unsigned char* g, unsigned char* b, unsigned char* a);

    SGL_API void SetEntityVisible(int handle, int visible);
    SGL_API SGL_BOOL  GetEntityVisible(int handle);

    SGL_API void SetEntityMeshID(int handle, int meshID);

    SGL_API SGL_BOOL IsEntityValid(int handle);

    SGL_API void SetEntityCollide(int handle, int canCollide);
    SGL_API SGL_BOOL  GetEntityCollide(int handle);

    SGL_API SGL_BOOL  CheckCollision(int a, int b);

#ifdef __cplusplus
}
#endif