#include "SGL.h"
#include "camera.h"
#include "core.h"

void Graphics3D(int width, int height, const char* title)
{
    raylib::InitWindow(width, height, title);
}

SGL_BOOL IsWindowShouldClose()
{
    return raylib::WindowShouldClose() ? 1 : 0;
}

void RenderWorld()
{
    SGL::InternalCamera& camera = SGL::GetCameraInstance();
    camera.UpdateCamera();

    raylib::BeginDrawing();

    raylib::ClearBackground({ 0, 0, 0, 255 });

    raylib::BeginMode3D(camera.raylibCamera);

    raylib::EndMode3D();

    raylib::EndDrawing();
}