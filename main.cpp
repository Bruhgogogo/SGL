#include "SGL.h"

#include <iostream>

using namespace std;

int main()
{
    Graphics3D(800, 600, "SGL Raycast Test");

    SetCameraPosition(0.0f, 3.0f, 8.0f);
    SetCameraRotation(-15.0f, 0.0f, 0.0f);

    int ground = CreateCube();
    SetEntityPosition(ground, 0.0f, -1.0f, 0.0f);
    SetEntityScale(ground, 10.0f, 1.0f, 10.0f);
    SetEntityColor(ground, 80, 80, 80, 255);

    int boxA = CreateCube();
    SetEntityPosition(boxA, 0.0f, 0.0f, 0.0f);
    SetEntityColor(boxA, 255, 80, 80, 255);

    int boxB = CreateCube();
    SetEntityPosition(boxB, 2.0f, 0.0f, 0.0f);
    SetEntityRotation(boxB, 0.0f, 45.0f, 0.0f);
    SetEntityColor(boxB, 80, 255, 80, 255);

    int boxC = CreateCube();
    SetEntityPosition(boxC, 0.0f, 1.5f, -4.0f);
    SetEntityRotation(boxC, 30.0f, 30.0f, 0.0f);
    SetEntityColor(boxC, 80, 80, 255, 255);

    while (!IsWindowShouldClose())
    {
        SGL_Ray ray;
        ray.startX = 0.0f;
        ray.startY = 0.0f;
        ray.startZ = 8.0f;
        ray.endX = 0.0f;
        ray.endY = 0.0f;
        ray.endZ = -8.0f;

        SGL_RayHit hit = Raycast(ray);

        if (hit.hit)
        {
            cout << "hit entity=" << hit.entity
                << " point=(" << hit.pointX << ", " << hit.pointY << ", " << hit.pointZ << ")"
                << " normal=(" << hit.normalX << ", " << hit.normalY << ", " << hit.normalZ << ")"
                << " dist=" << hit.distance
                << endl;
        }
        else
        {
            cout << "no hit" << endl;
        }

        RenderWorld();
    }

    return 0;
}