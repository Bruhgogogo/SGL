#include "SGL.h"

#include <cmath>
#include <iostream>

int main()
{
    Graphics3D(1270, 800, "Hello SGL!");

    int cube1 = CreateCube();
    SetEntityPosition(cube1, 0, 0, -5);

    int cube2 = CreateCube();
    SetEntityPosition(cube2, 0, 0, -5);
    SetEntityScale(cube2, 2, 1, 1);

    SetCameraPosition(0, 0, 0);

    float totalTime = 0.0;
    int lastHit = -1;

    while (!IsWindowShouldClose())
    {
        totalTime += 1.0f / 60.0f;

        float x = sinf(totalTime) * 2.0f;
        float angle = totalTime * 90.0f;

        SetEntityPosition(cube1, -x, 0, -5);
        SetEntityPosition(cube2, x, 0, -5);
        SetEntityRotation(cube2, 0, angle, 0);

        SGL_BOOL hit = CheckCollision(cube1, cube2);

        if (hit != lastHit)
        {
            if (hit)
                std::cout << "collision: YES" << std::endl;
            else
                std::cout << "collision: NO" << std::endl;

            lastHit = hit;
        }

        RenderWorld();
    }

    return 0;
}