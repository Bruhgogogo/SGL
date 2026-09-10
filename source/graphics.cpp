#include "graphics.h"
#include "camera.h"

namespace SGL
{

    void GraphicsInit(int width, int height, const char* title)
    {
        raylib::InitWindow(width, height, title);
    }

    bool GraphicsShouldClose()
    {
        return raylib::WindowShouldClose();
    }

    void GraphicsRender()
    {
        InternalCamera& camera = GetCameraInstance();
        camera.UpdateCamera();

        raylib::BeginDrawing();

        raylib::ClearBackground({ 0, 0, 0, 255 });

        raylib::BeginMode3D(camera.raylibCamera);

        raylib::EndMode3D();

        raylib::EndDrawing();
    }

}