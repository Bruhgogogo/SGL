#pragma once

#include "raylibUtils.h"
#include "component.h"
#include "SGL.h"

// ====================================================================================================
// NAMESPACE
// ====================================================================================================

namespace SGL
{
    // ====================================================================================================
    // APIs
    // ====================================================================================================

    void PhysicalComputeOBB(const Transform& world, OBB& obb);
    int  PhysicalCheckCollision(int a, int b);
    SGL_RayHit PhysicalRaycast(SGL_Ray ray);

}