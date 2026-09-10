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
    bool GraphicsShouldClose();
    void GraphicsGetWindowWidth(int* width);
    void GraphicsGetWindowHeight(int* height);
    void GraphicsSetWindowMode(int mode);
    void GraphicsSetWindowWidth(int width);
    void GraphicsSetWindowHeight(int height);
    void GraphicsGetWindowMode(int* mode);
    void GraphicsRender();

}