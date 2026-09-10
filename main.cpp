#include "SGL.h"

#include <cmath>

int main()
{
    Graphics3D(1270, 800, "Hello SGL!");

    int cube1 = CreateCube();
    SetEntityPosition(cube1, -2, 0, -5);

    int cube2 = CreateCube();
    SetEntityPosition(cube2, 2, 0, -5);

    SetParent(cube2, cube1);
    
    SetCameraPosition(0, 0, 0);

    float totalTime = 0.0;

    while (!IsWindowShouldClose())
    {
        totalTime += 1.0f / 60.0f;

        SetEntityPosition(cube1, -2, sinf(totalTime), -5);

        RenderWorld();
    }

    return 0;
}