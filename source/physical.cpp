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

    static bool OverlapOnAxis(
        const Vector3& aCenter, const Vector3& aHalf,
        const Vector3& aAxisX, const Vector3& aAxisY, const Vector3& aAxisZ,
        const Vector3& bCenter, const Vector3& bHalf,
        const Vector3& bAxisX, const Vector3& bAxisY, const Vector3& bAxisZ,
        const Vector3& axis)
    {
        float ra =
            fabsf(Vector3DotProduct(axis, aAxisX)) * aHalf.x +
            fabsf(Vector3DotProduct(axis, aAxisY)) * aHalf.y +
            fabsf(Vector3DotProduct(axis, aAxisZ)) * aHalf.z;

        float rb =
            fabsf(Vector3DotProduct(axis, bAxisX)) * bHalf.x +
            fabsf(Vector3DotProduct(axis, bAxisY)) * bHalf.y +
            fabsf(Vector3DotProduct(axis, bAxisZ)) * bHalf.z;

        Vector3 d = Vector3Subtract(bCenter, aCenter);
        float dist = fabsf(Vector3DotProduct(d, axis));

        return dist <= ra + rb;
    }

    static bool OBBIntersect(const OBB& a, const OBB& b)
    {
        Vector3 aX = Vector3RotateByQuaternion({ 1, 0, 0 }, a.worldRotation);
        Vector3 aY = Vector3RotateByQuaternion({ 0, 1, 0 }, a.worldRotation);
        Vector3 aZ = Vector3RotateByQuaternion({ 0, 0, 1 }, a.worldRotation);

        Vector3 bX = Vector3RotateByQuaternion({ 1, 0, 0 }, b.worldRotation);
        Vector3 bY = Vector3RotateByQuaternion({ 0, 1, 0 }, b.worldRotation);
        Vector3 bZ = Vector3RotateByQuaternion({ 0, 0, 1 }, b.worldRotation);

        if (!OverlapOnAxis(a.worldCenter, a.worldHalfExtents, aX, aY, aZ,
            b.worldCenter, b.worldHalfExtents, bX, bY, bZ, aX)) return false;
        if (!OverlapOnAxis(a.worldCenter, a.worldHalfExtents, aX, aY, aZ,
            b.worldCenter, b.worldHalfExtents, bX, bY, bZ, aY)) return false;
        if (!OverlapOnAxis(a.worldCenter, a.worldHalfExtents, aX, aY, aZ,
            b.worldCenter, b.worldHalfExtents, bX, bY, bZ, aZ)) return false;

        if (!OverlapOnAxis(a.worldCenter, a.worldHalfExtents, aX, aY, aZ,
            b.worldCenter, b.worldHalfExtents, bX, bY, bZ, bX)) return false;
        if (!OverlapOnAxis(a.worldCenter, a.worldHalfExtents, aX, aY, aZ,
            b.worldCenter, b.worldHalfExtents, bX, bY, bZ, bY)) return false;
        if (!OverlapOnAxis(a.worldCenter, a.worldHalfExtents, aX, aY, aZ,
            b.worldCenter, b.worldHalfExtents, bX, bY, bZ, bZ)) return false;

        Vector3 cross;

        cross = Vector3CrossProduct(aX, bX);
        if (Vector3Length(cross) > 1e-6f && !OverlapOnAxis(a.worldCenter, a.worldHalfExtents, aX, aY, aZ,
            b.worldCenter, b.worldHalfExtents, bX, bY, bZ, cross)) return false;

        cross = Vector3CrossProduct(aX, bY);
        if (Vector3Length(cross) > 1e-6f && !OverlapOnAxis(a.worldCenter, a.worldHalfExtents, aX, aY, aZ,
            b.worldCenter, b.worldHalfExtents, bX, bY, bZ, cross)) return false;

        cross = Vector3CrossProduct(aX, bZ);
        if (Vector3Length(cross) > 1e-6f && !OverlapOnAxis(a.worldCenter, a.worldHalfExtents, aX, aY, aZ,
            b.worldCenter, b.worldHalfExtents, bX, bY, bZ, cross)) return false;

        cross = Vector3CrossProduct(aY, bX);
        if (Vector3Length(cross) > 1e-6f && !OverlapOnAxis(a.worldCenter, a.worldHalfExtents, aX, aY, aZ,
            b.worldCenter, b.worldHalfExtents, bX, bY, bZ, cross)) return false;

        cross = Vector3CrossProduct(aY, bY);
        if (Vector3Length(cross) > 1e-6f && !OverlapOnAxis(a.worldCenter, a.worldHalfExtents, aX, aY, aZ,
            b.worldCenter, b.worldHalfExtents, bX, bY, bZ, cross)) return false;

        cross = Vector3CrossProduct(aY, bZ);
        if (Vector3Length(cross) > 1e-6f && !OverlapOnAxis(a.worldCenter, a.worldHalfExtents, aX, aY, aZ,
            b.worldCenter, b.worldHalfExtents, bX, bY, bZ, cross)) return false;

        cross = Vector3CrossProduct(aZ, bX);
        if (Vector3Length(cross) > 1e-6f && !OverlapOnAxis(a.worldCenter, a.worldHalfExtents, aX, aY, aZ,
            b.worldCenter, b.worldHalfExtents, bX, bY, bZ, cross)) return false;

        cross = Vector3CrossProduct(aZ, bY);
        if (Vector3Length(cross) > 1e-6f && !OverlapOnAxis(a.worldCenter, a.worldHalfExtents, aX, aY, aZ,
            b.worldCenter, b.worldHalfExtents, bX, bY, bZ, cross)) return false;

        cross = Vector3CrossProduct(aZ, bZ);
        if (Vector3Length(cross) > 1e-6f && !OverlapOnAxis(a.worldCenter, a.worldHalfExtents, aX, aY, aZ,
            b.worldCenter, b.worldHalfExtents, bX, bY, bZ, cross)) return false;

        return true;
    }

    static bool RayOBBIntersect(
        SGL_Ray ray,
        const OBB& obb,
        float& tHit,
        Vector3& normal)
    {
        Vector3 rayStart = { ray.startX, ray.startY, ray.startZ };
        Vector3 rayEnd = { ray.endX, ray.endY, ray.endZ };

        Quaternion invRot = QuaternionInvert(obb.worldRotation);

        Vector3 localStart = Vector3Subtract(rayStart, obb.worldCenter);
        localStart = Vector3RotateByQuaternion(localStart, invRot);

        Vector3 localEnd = Vector3Subtract(rayEnd, obb.worldCenter);
        localEnd = Vector3RotateByQuaternion(localEnd, invRot);

        Vector3 dir = Vector3Subtract(localEnd, localStart);

        float tmin = 0.0f;
        float tmax = 1.0f;

        int hitAxis = -1;
        float hitSign = 1.0f;

        for (int axis = 0; axis < 3; axis++)
        {
            float origin = axis == 0 ? localStart.x : (axis == 1 ? localStart.y : localStart.z);
            float direction = axis == 0 ? dir.x : (axis == 1 ? dir.y : dir.z);
            float half = axis == 0 ? obb.worldHalfExtents.x : (axis == 1 ? obb.worldHalfExtents.y : obb.worldHalfExtents.z);

            if (fabsf(direction) < 1e-8f)
            {
                if (origin < -half || origin > half)
                    return false;
                continue;
            }

            float invDir = 1.0f / direction;
            float t1 = (-half - origin) * invDir;
            float t2 = (half - origin) * invDir;

            float sign = -1.0f;

            if (t1 > t2)
            {
                float tmp = t1;
                t1 = t2;
                t2 = tmp;
                sign = 1.0f;
            }

            if (t1 > tmin)
            {
                tmin = t1;
                hitAxis = axis;
                hitSign = sign;
            }

            if (t2 < tmax)
                tmax = t2;

            if (tmin > tmax)
                return false;
        }

        tHit = tmin;

        if (hitAxis == 0)
            normal = { hitSign, 0, 0 };
        else if (hitAxis == 1)
            normal = { 0, hitSign, 0 };
        else
            normal = { 0, 0, hitSign };

        normal = Vector3RotateByQuaternion(normal, obb.worldRotation);

        return true;
    }

    // ====================================================================================================
    // APIs
    // ====================================================================================================

    void PhysicalComputeOBB(const Transform& world, OBB& obb)
    {
        Vector3 scaledCenter = {
            obb.localCenter.x * world.scale.x,
            obb.localCenter.y * world.scale.y,
            obb.localCenter.z * world.scale.z
        };

        Vector3 rotatedCenter = Vector3RotateByQuaternion(scaledCenter, world.rotation);
        obb.worldCenter = Vector3Add(world.position, rotatedCenter);

        obb.worldHalfExtents = {
            obb.localHalfExtents.x * world.scale.x,
            obb.localHalfExtents.y * world.scale.y,
            obb.localHalfExtents.z * world.scale.z
        };

        obb.worldRotation = QuaternionMultiply(world.rotation, obb.localRotation);
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

        const OBB& boxA = scene.GetComponent<OBB>(a);
        const OBB& boxB = scene.GetComponent<OBB>(b);

        return OBBIntersect(boxA, boxB) ? 1 : 0;
    }

    SGL_RayHit PhysicalRaycast(SGL_Ray ray)
    {
        Scene& scene = GetSceneInstance();

        SGL_RayHit result = {};
        result.entity = -1;

        float closest = 1e30f;

        Vector3 rayStart = { ray.startX, ray.startY, ray.startZ };
        Vector3 rayEnd = { ray.endX, ray.endY, ray.endZ };
        Vector3 rayDelta = Vector3Subtract(rayEnd, rayStart);

        for (int i = 0; i < (int)scene.GetEntityCount(); i++)
        {
            if (!scene.IsAlive(i)) continue;
            if (!scene.HasComponent<OBB>(i)) continue;
            if (!scene.HasComponent<Physical>(i)) continue;
            if (scene.GetComponent<Physical>(i).canCollide == 0) continue;

            const OBB& obb = scene.GetComponent<OBB>(i);

            float tHit = 0.0f;
            Vector3 normal = { 0, 0, 0 };

            if (!RayOBBIntersect(ray, obb, tHit, normal))
                continue;

            if (tHit >= closest)
                continue;

            closest = tHit;

            Vector3 point = Vector3Add(rayStart, Vector3Scale(rayDelta, tHit));

            result.hit = 1;
            result.entity = i;
            result.pointX = point.x;
            result.pointY = point.y;
            result.pointZ = point.z;
            result.normalX = normal.x;
            result.normalY = normal.y;
            result.normalZ = normal.z;
            result.distance = Vector3Distance(rayStart, point);
        }

        return result;
    }

}