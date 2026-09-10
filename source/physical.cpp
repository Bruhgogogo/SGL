#include "physical.h"
#include "scene.h"

// ====================================================================================================
// NAMESPACE
// ====================================================================================================

namespace SGL
{
    // ====================================================================================================
    // STATIC
    // ====================================================================================================

    static OBB GetMeshBounds(int meshID)
    {
        OBB box;

        if (meshID == 0)
        {
            box.center = { 0, 0, 0 };
            box.halfExtents = { 0.5f, 0.5f, 0.5f };
            box.rotation = QuaternionIdentity();
        }
        else
        {
            box.center = { 0, 0, 0 };
            box.halfExtents = { 0, 0, 0 };
            box.rotation = QuaternionIdentity();
        }

        return box;
    }

    static bool OverlapOnAxis(
        const OBB& a, const OBB& b,
        const Vector3& axis,
        const Vector3& aAxisX, const Vector3& aAxisY, const Vector3& aAxisZ,
        const Vector3& bAxisX, const Vector3& bAxisY, const Vector3& bAxisZ)
    {
        float ra =
            fabsf(Vector3DotProduct(axis, aAxisX)) * a.halfExtents.x +
            fabsf(Vector3DotProduct(axis, aAxisY)) * a.halfExtents.y +
            fabsf(Vector3DotProduct(axis, aAxisZ)) * a.halfExtents.z;

        float rb =
            fabsf(Vector3DotProduct(axis, bAxisX)) * b.halfExtents.x +
            fabsf(Vector3DotProduct(axis, bAxisY)) * b.halfExtents.y +
            fabsf(Vector3DotProduct(axis, bAxisZ)) * b.halfExtents.z;

        Vector3 d = Vector3Subtract(b.center, a.center);
        float dist = fabsf(Vector3DotProduct(d, axis));

        return dist <= ra + rb;
    }

    static bool OBBIntersect(const OBB& a, const OBB& b)
    {
        Vector3 aX = Vector3RotateByQuaternion({ 1, 0, 0 }, a.rotation);
        Vector3 aY = Vector3RotateByQuaternion({ 0, 1, 0 }, a.rotation);
        Vector3 aZ = Vector3RotateByQuaternion({ 0, 0, 1 }, a.rotation);

        Vector3 bX = Vector3RotateByQuaternion({ 1, 0, 0 }, b.rotation);
        Vector3 bY = Vector3RotateByQuaternion({ 0, 1, 0 }, b.rotation);
        Vector3 bZ = Vector3RotateByQuaternion({ 0, 0, 1 }, b.rotation);

        if (!OverlapOnAxis(a, b, aX, aX, aY, aZ, bX, bY, bZ)) return false;
        if (!OverlapOnAxis(a, b, aY, aX, aY, aZ, bX, bY, bZ)) return false;
        if (!OverlapOnAxis(a, b, aZ, aX, aY, aZ, bX, bY, bZ)) return false;

        if (!OverlapOnAxis(a, b, bX, aX, aY, aZ, bX, bY, bZ)) return false;
        if (!OverlapOnAxis(a, b, bY, aX, aY, aZ, bX, bY, bZ)) return false;
        if (!OverlapOnAxis(a, b, bZ, aX, aY, aZ, bX, bY, bZ)) return false;

        Vector3 cross;

        cross = Vector3CrossProduct(aX, bX);
        if (Vector3Length(cross) > 1e-6f && !OverlapOnAxis(a, b, cross, aX, aY, aZ, bX, bY, bZ)) return false;

        cross = Vector3CrossProduct(aX, bY);
        if (Vector3Length(cross) > 1e-6f && !OverlapOnAxis(a, b, cross, aX, aY, aZ, bX, bY, bZ)) return false;

        cross = Vector3CrossProduct(aX, bZ);
        if (Vector3Length(cross) > 1e-6f && !OverlapOnAxis(a, b, cross, aX, aY, aZ, bX, bY, bZ)) return false;

        cross = Vector3CrossProduct(aY, bX);
        if (Vector3Length(cross) > 1e-6f && !OverlapOnAxis(a, b, cross, aX, aY, aZ, bX, bY, bZ)) return false;

        cross = Vector3CrossProduct(aY, bY);
        if (Vector3Length(cross) > 1e-6f && !OverlapOnAxis(a, b, cross, aX, aY, aZ, bX, bY, bZ)) return false;

        cross = Vector3CrossProduct(aY, bZ);
        if (Vector3Length(cross) > 1e-6f && !OverlapOnAxis(a, b, cross, aX, aY, aZ, bX, bY, bZ)) return false;

        cross = Vector3CrossProduct(aZ, bX);
        if (Vector3Length(cross) > 1e-6f && !OverlapOnAxis(a, b, cross, aX, aY, aZ, bX, bY, bZ)) return false;

        cross = Vector3CrossProduct(aZ, bY);
        if (Vector3Length(cross) > 1e-6f && !OverlapOnAxis(a, b, cross, aX, aY, aZ, bX, bY, bZ)) return false;

        cross = Vector3CrossProduct(aZ, bZ);
        if (Vector3Length(cross) > 1e-6f && !OverlapOnAxis(a, b, cross, aX, aY, aZ, bX, bY, bZ)) return false;

        return true;
    }

    // ====================================================================================================
    // APIs
    // ====================================================================================================

    OBB PhysicalComputeOBB(const Transform& world, int meshID)
    {
        OBB local = GetMeshBounds(meshID);
        OBB result;

        Vector3 scaledCenter = {
            local.center.x * world.scale.x,
            local.center.y * world.scale.y,
            local.center.z * world.scale.z
        };

        Vector3 rotatedCenter = Vector3RotateByQuaternion(scaledCenter, world.rotation);
        result.center = Vector3Add(world.position, rotatedCenter);

        result.halfExtents = {
            local.halfExtents.x * world.scale.x,
            local.halfExtents.y * world.scale.y,
            local.halfExtents.z * world.scale.z
        };

        result.rotation = QuaternionMultiply(world.rotation, local.rotation);

        return result;
    }

    int PhysicalCheckCollision(int a, int b)
    {
        Scene& scene = GetSceneInstance();

        if (!scene.IsAlive(a) || !scene.IsAlive(b)) return 0;

        if (!scene.HasComponent<OBB>(a)) return 0;
        if (!scene.HasComponent<OBB>(b)) return 0;

        if (!scene.HasComponent<Physical>(a)) return 0;
        if (!scene.HasComponent<Physical>(b)) return 0;

        if (scene.GetComponent<Physical>(a).canCollide == 0) return 0;
        if (scene.GetComponent<Physical>(b).canCollide == 0) return 0;

        OBB boxA = scene.GetComponent<OBB>(a);
        OBB boxB = scene.GetComponent<OBB>(b);

        return OBBIntersect(boxA, boxB) ? 1 : 0;
    }

}