#pragma once

#include "raylibUtils.h"

// ====================================================================================================
// NAMESPACE
// ====================================================================================================

namespace SGL
{
    // ====================================================================================================
    // APIs
    // ====================================================================================================

    void GraphicsInit(int width, int height, const char* title);
    void GraphicsShutdown();
    bool GraphicsShouldClose();
    void GraphicsGetWindowWidth(int* width);
    void GraphicsGetWindowHeight(int* height);
    void GraphicsSetWindowMode(int mode);
    void GraphicsSetWindowWidth(int width);
    void GraphicsSetWindowHeight(int height);
    void GraphicsGetWindowMode(int* mode);

    void GraphicsSetGroundAmbient(float r, float g, float b);
    void GraphicsGetGroundAmbient(float* r, float* g, float* b);
    void GraphicsSetSkyAmbient(float r, float g, float b);
    void GraphicsGetSkyAmbient(float* r, float* g, float* b);

    int GetBuiltinCubeMeshHandle();
    int GetBuiltinSphereMeshHandle();
    int GetBuiltinCapsuleMeshHandle();
    int GetBuiltinCylinderMeshHandle();

    void AABBUpdateWorld(int handle);

    void GraphicsRender();

}