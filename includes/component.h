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
        float mass = 1.0f;
        int canCollide = 1;
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