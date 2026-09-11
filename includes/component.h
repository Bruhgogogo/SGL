#pragma once

#include "raylibUtils.h"

namespace SGL
{
    struct Transform
    {
        Vector3    position = { 0, 0, 0 };
        Quaternion rotation = QuaternionIdentity();
        Vector3    scale = { 1, 1, 1 };

        mutable Matrix cachedMatrix = MatrixIdentity();
        mutable int    matrixDirty = 1;
    };

    struct Color
    {
        unsigned char r = 255;
        unsigned char g = 255;
        unsigned char b = 255;
        unsigned char a = 255;
    };

    struct Visible
    {
        int value = 1;
    };

    struct Physical
    {
        int   anchored = 1;
        int   canCollide = 1;
        int   allowSleeping = 1;
        int   isSensor = 0;

        float friction = 0.5f;
        float restitution = 0.0f;
        float linearDamping = 0.05f;
        float angularDamping = 0.05f;
        float gravityFactor = 1.0f;
        float mass = 1.0f;
    };

    struct OBB
    {
        Vector3    localCenter = { 0, 0, 0 };
        Vector3    localHalfExtents = { 0, 0, 0 };
        Quaternion localRotation = QuaternionIdentity();

        Vector3    worldCenter = { 0, 0, 0 };
        Vector3    worldHalfExtents = { 0, 0, 0 };
        Quaternion worldRotation = QuaternionIdentity();
    };

    struct Sphere
    {
        Vector3 localCenter = { 0, 0, 0 };
        float   localRadius = 0.5f;

        Vector3 worldCenter = { 0, 0, 0 };
        Vector3 worldRadii = { 0.5f, 0.5f, 0.5f };
        float   worldMaxRadius = 0.5f;
    };

    struct Capsule
    {
        Vector3    localCenter = { 0, 0, 0 };
        float      localRadius = 0.5f;
        float      localHeight = 1.0f;

        Vector3    worldCenter = { 0, 0, 0 };
        float      worldRadius = 0.5f;
        float      worldHeight = 1.0f;
        Quaternion worldRotation = QuaternionIdentity();
    };

    struct Cylinder
    {
        Vector3    localCenter = { 0, 0, 0 };
        float      localRadius = 0.5f;
        float      localHeight = 1.0f;

        Vector3    worldCenter = { 0, 0, 0 };
        float      worldRadius = 0.5f;
        float      worldHeight = 1.0f;
        Quaternion worldRotation = QuaternionIdentity();
    };

    struct MeshID
    {
        int value = 0;
    };

    struct RenderItem
    {
        Transform transform;
        Color     color;
        int       meshID;
    };
}