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
    Graphics3D(800, 600, "SGL Capsule Cylinder Test");

    SetCameraPosition(0.0f, 6.0f, 16.0f);
    SetCameraRotation(-20.0f, 0.0f, 0.0f);

    int cylA = CreateCylinder();
    SetEntityPosition(cylA, -6.0f, 0.0f, 0.0f);
    SetEntityScale(cylA, 1.0f, 2.0f, 1.0f);
    SetEntityColor(cylA, 255, 80, 80, 255);

    int cylB = CreateCylinder();
    SetEntityPosition(cylB, -3.0f, 0.0f, 0.0f);
    SetEntityScale(cylB, 1.0f, 2.0f, 1.0f);
    SetEntityColor(cylB, 255, 160, 80, 255);

    int capA = CreateCapsule();
    SetEntityPosition(capA, 0.0f, 0.0f, 0.0f);
    SetEntityScale(capA, 1.0f, 2.0f, 1.0f);
    SetEntityColor(capA, 80, 255, 80, 255);

    int capB = CreateCapsule();
    SetEntityPosition(capB, 3.0f, 0.0f, 0.0f);
    SetEntityScale(capB, 1.0f, 2.0f, 1.0f);
    SetEntityColor(capB, 80, 255, 160, 255);

    int cylC = CreateCylinder();
    SetEntityPosition(cylC, 6.0f, 0.0f, 0.0f);
    SetEntityScale(cylC, 1.0f, 2.0f, 1.0f);
    SetEntityColor(cylC, 80, 80, 255, 255);

    cout << "=== capsule vs capsule ===" << endl;
    cout << "capA vs capB = " << CheckCollision(capA, capB) << endl;

    cout << "=== cylinder vs cylinder ===" << endl;
    cout << "cylA vs cylB = " << CheckCollision(cylA, cylB) << endl;

    cout << "=== capsule vs cylinder ===" << endl;
    cout << "capA vs cylC = " << CheckCollision(capA, cylC) << endl;

    cout << "=== cylinder vs capsule ===" << endl;
    cout << "cylA vs capA = " << CheckCollision(cylA, capA) << endl;

    cout << "=== ray vs capsule (through capA center) ===" << endl;
    PrintHit("capA", Raycast(MakeRay(0.0f, 0.0f, 8.0f, 0.0f, 0.0f, -8.0f)));

    cout << "=== ray vs capsule (x=0.6, outside radius 0.5) ===" << endl;
    PrintHit("capA x=0.6", Raycast(MakeRay(0.6f, 0.0f, 8.0f, 0.6f, 0.0f, -8.0f)));

    cout << "=== ray vs capsule (x=0.4, inside radius 0.5) ===" << endl;
    PrintHit("capA x=0.4", Raycast(MakeRay(0.4f, 0.0f, 8.0f, 0.4f, 0.0f, -8.0f)));

    cout << "=== ray vs cylinder (through cylA center) ===" << endl;
    PrintHit("cylA", Raycast(MakeRay(-6.0f, 0.0f, 8.0f, -6.0f, 0.0f, -8.0f)));

    cout << "=== ray vs cylinder (x=-5.6, inside radius 0.5) ===" << endl;
    PrintHit("cylA x=-5.6", Raycast(MakeRay(-5.6f, 0.0f, 8.0f, -5.6f, 0.0f, -8.0f)));

    cout << "=== ray vs cylinder (x=-5.4, outside radius 0.5) ===" << endl;
    PrintHit("cylA x=-5.4", Raycast(MakeRay(-5.4f, 0.0f, 8.0f, -5.4f, 0.0f, -8.0f)));

    cout << "=== ray vs cylinder (y=0.9, inside half-height 1.0) ===" << endl;
    PrintHit("cylA y=0.9", Raycast(MakeRay(-6.0f, 0.9f, 8.0f, -6.0f, 0.9f, -8.0f)));

    cout << "=== ray vs cylinder (y=1.1, outside half-height 1.0) ===" << endl;
    PrintHit("cylA y=1.1", Raycast(MakeRay(-6.0f, 1.1f, 8.0f, -6.0f, 1.1f, -8.0f)));

    cout << "=== ray vs cylinder (top cap, along -Y) ===" << endl;
    PrintHit("cylA top", Raycast(MakeRay(-6.0f, 8.0f, 0.0f, -6.0f, -8.0f, 0.0f)));

    while (!IsWindowShouldClose())
    {
        RenderWorld();
    }

    return 0;
}