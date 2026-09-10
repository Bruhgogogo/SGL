#pragma once

typedef int SGL_BOOL;

void Graphics3D(int width, int height, const char* title);
SGL_BOOL IsWindowShouldClose();
void RenderWorld();

void SetCameraPosition(float x, float y, float z);
void GetCameraPosition(float* x, float* y, float* z);
void SetCameraRotation(float pitch, float yaw, float roll);
void GetCameraRotation(float* pitch, float* yaw, float* roll);
void SetCameraFOV(float fov);
void GetCameraFOV(float* fov);