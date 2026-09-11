#include "SGL.h"

#include <iostream>

using namespace std;

static void PrintHit(const char* label, SGL_RayHit hit)
{
    if (hit.hit)
    {
        cout << label << " hit entity=" << hit.entity
            << " point=(" << hit.pointX << ", " << hit.pointY << ", " << hit.pointZ << ")"
            << " normal=(" << hit.normalX << ", " << hit.normalY << ", " << hit.normalZ << ")"
            << " dist=" << hit.distance
            << endl;
    }
    else
    {
        cout << label << " no hit" << endl;
    }
}

static SGL_Ray MakeRay(float sx, float sy, float sz, float ex, float ey, float ez)
{
    SGL_Ray ray;
    ray.startX = sx;
    ray.startY = sy;
    ray.startZ = sz;
    ray.endX = ex;
    ray.endY = ey;
    ray.endZ = ez;
    return ray;
}

int main()
{
    Graphics3D(800, 600, "SGL Sphere Test");

    SetCameraPosition(0.0f, 4.0f, 12.0f);
    SetCameraRotation(-15.0f, 0.0f, 0.0f);

    int sphereUniform = CreateSphere();
    SetEntityPosition(sphereUniform, -1.0f, 0.0f, 0.0f);
    SetEntityScale(sphereUniform, 1.0f, 1.0f, 1.0f);
    SetEntityColor(sphereUniform, 255, 80, 80, 255);

    int sphereStretchedY = CreateSphere();
    SetEntityPosition(sphereStretchedY, 0.0f, 0.0f, 0.0f);
    SetEntityScale(sphereStretchedY, 1.0f, 3.0f, 1.0f);
    SetEntityColor(sphereStretchedY, 80, 255, 80, 255);

    int sphereStretchedXZ = CreateSphere();
    SetEntityPosition(sphereStretchedXZ, 1.0f, 0.0f, 0.0f);
    SetEntityScale(sphereStretchedXZ, 3.0f, 1.0f, 1.0f);
    SetEntityColor(sphereStretchedXZ, 80, 80, 255, 255);

    int cube = CreateCube();
    SetEntityPosition(cube, 0.0f, 4.0f, 0.0f);
    SetEntityColor(cube, 255, 255, 80, 255);

    cout << "=== uniform sphere: ray along -Z through center ===" << endl;
    PrintHit("uniform", Raycast(MakeRay(-1.0f, 0.0f, 8.0f, -1.0f, 0.0f, -8.0f)));

    cout << "=== stretched Y sphere: ray along -Z through center ===" << endl;
    PrintHit("stretchedY", Raycast(MakeRay(0.0f, 0.0f, 8.0f, 0.0f, 0.0f, -8.0f)));

    cout << "=== stretched Y sphere: ray through y=1.4 (should hit) ===" << endl;
    PrintHit("stretchedY y=1.4", Raycast(MakeRay(0.0f, 1.4f, 8.0f, 0.0f, 1.4f, -8.0f)));

    cout << "=== stretched Y sphere: ray through y=1.6 (should miss) ===" << endl;
    PrintHit("stretchedY y=1.6", Raycast(MakeRay(0.0f, 1.6f, 8.0f, 0.0f, 1.6f, -8.0f)));

    cout << "=== stretched XZ sphere: ray along -Z through center ===" << endl;
    PrintHit("stretchedXZ", Raycast(MakeRay(1.0f, 0.0f, 8.0f, 1.0f, 0.0f, -8.0f)));

    cout << "=== stretched XZ sphere: ray through x=2.4 (should hit) ===" << endl;
    PrintHit("stretchedXZ x=2.4", Raycast(MakeRay(2.4f, 0.0f, 8.0f, 2.4f, 0.0f, -8.0f)));

    cout << "=== stretched XZ sphere: ray through x=2.6 (should miss) ===" << endl;
    PrintHit("stretchedXZ x=2.6", Raycast(MakeRay(2.6f, 0.0f, 8.0f, 2.6f, 0.0f, -8.0f)));

    cout << "=== cube at y=4: ray along -Z through center ===" << endl;
    PrintHit("cube", Raycast(MakeRay(0.0f, 4.0f, 8.0f, 0.0f, 4.0f, -8.0f)));

    cout << "=== collision tests (conservative, max scale) ===" << endl;
    cout << "uniform vs stretchedY = " << CheckCollision(sphereUniform, sphereStretchedY) << endl;
    cout << "stretchedY vs stretchedXZ = " << CheckCollision(sphereStretchedY, sphereStretchedXZ) << endl;
    cout << "stretchedY vs cube = " << CheckCollision(sphereStretchedY, cube) << endl;

    while (!IsWindowShouldClose())
    {
        RenderWorld();
    }

    return 0;
}