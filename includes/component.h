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

    struct DirectionalLight
    {
        float intensity = 1.0f;
        int   shadow = 0;
        int   specular = 0;
    };

    struct PointLight
    {
        float intensity = 1.0f;
        float range = 10.0f;
        int   shadow = 0;
        int   specular = 0;
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

    struct BoxShape
    {
        Vector3 localHalfExtents = { 0.5f, 0.5f, 0.5f };

        Vector3 worldHalfExtents = { 0.5f, 0.5f, 0.5f };

        int dirty = 1;
    };

    struct SphereShape
    {
        float   localRadius = 0.5f;

        float   worldRadius = 0.5f;

        int dirty = 1;
    };

    struct CapsuleShape
    {
        float   localRadius = 0.5f;
        float   localHeight = 1.0f;

        float   worldRadius = 0.5f;
        float   worldHeight = 1.0f;

        int dirty = 1;
    };

    struct CylinderShape
    {
        float   localRadius = 0.5f;
        float   localHeight = 1.0f;

        float   worldRadius = 0.5f;
        float   worldHeight = 1.0f;

        int dirty = 1;
    };

    struct AABB
    {
        Vector3 worldMin = { 0, 0, 0 };
        Vector3 worldMax = { 0, 0, 0 };

        int dirty = 1;
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

    struct DirectionalLightItem
    {
        Vector3 direction;
        Vector3 color;
        float   intensity;
    };

    struct PointLightItem
    {
        Vector3 position;
        Vector3 color;
        float   intensity;
        float   range;
    };
}