#include "SGL.h"

#include <iostream>

using namespace std;

constexpr float deg2rad = 3.14159265358979323846f / 180.0f;

static float DegToRad(float deg)
{
    return deg * deg2rad;
}

int main()
{
    Graphics3D(800, 600, "SGL Jolt Physics Test");

    SetCameraPosition(0.0f, 8.0f, 16.0f);
    SetCameraRotation(DegToRad(-25.0f), 0.0f, 0.0f);

    SetGroundAmbient(0.2f, 0.15f, 0.1f);
    SetSkyAmbient(0.5f, 0.6f, 0.8f);

    int spot = CreateSpotLight();
    SetEntityPosition(spot, 0.0f, 8.0f, 4.0f);
    SetEntityRotation(spot, DegToRad(-60.0f), 0.0f, 0.0f);
    SetEntityColor(spot, 120, 180, 255, 255);
    SetLightIntensity(spot, 5.0f);
    SetLightRange(spot, 20.0f);
    SetLightInnerAngle(spot, DegToRad(20.0f));
    SetLightOuterAngle(spot, DegToRad(35.0f));
     
    int ground = CreateCube();
    SetEntityPosition(ground, 0.0f, -1.0f, 0.0f);
    SetEntityScale(ground, 20.0f, 1.0f, 20.0f);
    SetEntityColor(ground, 80, 80, 80, 255);

    int ramp = CreateCube();
    SetEntityPosition(ramp, 4.0f, 1.0f, 0.0f);
    SetEntityRotation(ramp, 0.0f, 0.0f, DegToRad(-30.0f));
    SetEntityScale(ramp, 4.0f, 0.5f, 4.0f);
    SetEntityColor(ramp, 120, 120, 120, 255);

    int boxA = CreateCube();
    SetEntityPosition(boxA, -2.0f, 6.0f, 0.0f);
    SetEntityColor(boxA, 255, 80, 80, 255);
    SetEntityAnchored(boxA, 0);

    int boxB = CreateCube();
    SetEntityPosition(boxB, -1.0f, 9.0f, 0.0f);
    SetEntityColor(boxB, 255, 160, 80, 255);
    SetEntityAnchored(boxB, 0);

    int boxC = CreateCube();
    SetEntityPosition(boxC, 0.0f, 12.0f, 0.0f);
    SetEntityColor(boxC, 255, 255, 80, 255);
    SetEntityAnchored(boxC, 0);

    int sphere = CreateSphere();
    SetEntityPosition(sphere, 2.0f, 6.0f, 0.0f);
    SetEntityColor(sphere, 80, 255, 80, 255);
    SetEntityAnchored(sphere, 0);

    int monkeyMesh = LoadMeshAsset("monkey.obj");

    int monkey = CreateMesh(monkeyMesh);
    SetEntityPosition(monkey, 4.0f, 8.0f, 0.0f);
    SetEntityColor(monkey, 80, 255, 160, 255);
    SetEntityAnchored(monkey, 0);

    while (!IsWindowShouldClose())
    {
        UpdateWorld(1);
        RenderWorld();
    }

    ShutdownSGL();

    return 0;
}