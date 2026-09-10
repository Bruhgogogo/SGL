#include "SGL.h"

int main()
{
    Graphics3D(1270, 800, "Hello Raylib!");

    int cube = CreateCube();
    SetEntityPosition(cube, 0, 0, -5);

    SetCameraPosition(0, 0, 0);

    while (!IsWindowShouldClose())
    {
        RenderWorld();
    }

    return 0;
}