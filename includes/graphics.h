#pragma once

#include "raylibUtils.h"

namespace SGL
{

    void GraphicsInit(int width, int height, const char* title);
    bool GraphicsShouldClose();
    void GraphicsRender();

}