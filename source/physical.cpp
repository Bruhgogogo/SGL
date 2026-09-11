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

    static bool RaySphereIntersect(
        SGL_Ray ray,
        const Sphere& sphere,
        float& tHit,
        Vector3& normal)
    {
        Vector3 rayStart = { ray.startX, ray.startY, ray.startZ };
        Vector3 rayEnd = { ray.endX, ray.endY, ray.endZ };

        Vector3 radii = sphere.worldRadii;

        if (radii.x < 1e-8f || radii.y < 1e-8f || radii.z < 1e-8f)
            return false;

        Vector3 invRadii = { 1.0f / radii.x, 1.0f / radii.y, 1.0f / radii.z };

        Vector3 localStart = Vector3Subtract(rayStart, sphere.worldCenter);
        localStart = { localStart.x * invRadii.x, localStart.y * invRadii.y, localStart.z * invRadii.z };

        Vector3 localEnd = Vector3Subtract(rayEnd, sphere.worldCenter);
        localEnd = { localEnd.x * invRadii.x, localEnd.y * invRadii.y, localEnd.z * invRadii.z };

        Vector3 dir = Vector3Subtract(localEnd, localStart);

        float a = Vector3DotProduct(dir, dir);
        float b = 2.0f * Vector3DotProduct(localStart, dir);
        float c = Vector3DotProduct(localStart, localStart) - 1.0f;

        float discriminant = b * b - 4.0f * a * c;

        if (discriminant < 0.0f)
            return false;

        float sqrtDisc = sqrtf(discriminant);
        float inv2a = 1.0f / (2.0f * a);

        float t0 = (-b - sqrtDisc) * inv2a;
        float t1 = (-b + sqrtDisc) * inv2a;

        float t = t0;

        if (t < 0.0f)
            t = t1;

        if (t < 0.0f || t > 1.0f)
            return false;

        Vector3 localPoint = Vector3Add(localStart, Vector3Scale(dir, t));

        Vector3 localNormal = localPoint;
        Vector3 worldNormal = {
            localNormal.x * invRadii.x,
            localNormal.y * invRadii.y,
            localNormal.z * invRadii.z
        };
        worldNormal = Vector3Normalize(worldNormal);

        tHit = t;
        normal = worldNormal;

        return true;
    }

    static bool SphereSphereIntersect(const Sphere& a, const Sphere& b)
    {
        float dist = Vector3Distance(a.worldCenter, b.worldCenter);
        return dist <= a.worldMaxRadius + b.worldMaxRadius;
    }

    static bool SphereOBBIntersect(const Sphere& s, const OBB& o)
    {
        Vector3 localCenter = Vector3Subtract(s.worldCenter, o.worldCenter);
        localCenter = Vector3RotateByQuaternion(localCenter, QuaternionInvert(o.worldRotation));

        Vector3 closest = {
            fmaxf(-o.worldHalfExtents.x, fminf(localCenter.x, o.worldHalfExtents.x)),
            fmaxf(-o.worldHalfExtents.y, fminf(localCenter.y, o.worldHalfExtents.y)),
            fmaxf(-o.worldHalfExtents.z, fminf(localCenter.z, o.worldHalfExtents.z))
        };

        float dist = Vector3Distance(localCenter, closest);

        return dist <= s.worldMaxRadius;
    }

    static void CapsuleSegment(const Capsule& c, Vector3& p0, Vector3& p1)
    {
        Vector3 axis = Vector3RotateByQuaternion({ 0, 1, 0 }, c.worldRotation);
        Vector3 half = Vector3Scale(axis, c.worldHeight * 0.5f);
        p0 = Vector3Subtract(c.worldCenter, half);
        p1 = Vector3Add(c.worldCenter, half);
    }

    static float SegmentSegmentDistance(
        const Vector3& p1, const Vector3& q1,
        const Vector3& p2, const Vector3& q2,
        Vector3& c1, Vector3& c2)
    {
        Vector3 d1 = Vector3Subtract(q1, p1);
        Vector3 d2 = Vector3Subtract(q2, p2);
        Vector3 r = Vector3Subtract(p1, p2);

        float a = Vector3DotProduct(d1, d1);
        float e = Vector3DotProduct(d2, d2);
        float f = Vector3DotProduct(d2, r);

        float s = 0.0f;
        float t = 0.0f;

        if (a <= 1e-8f && e <= 1e-8f)
        {
            c1 = p1;
            c2 = p2;
            return Vector3Distance(p1, p2);
        }

        if (a <= 1e-8f)
        {
            s = 0.0f;
            t = f / e;
            t = fmaxf(0.0f, fminf(1.0f, t));
        }
        else
        {
            float c = Vector3DotProduct(d1, r);

            if (e <= 1e-8f)
            {
                t = 0.0f;
                s = fmaxf(0.0f, fminf(1.0f, -c / a));
            }
            else
            {
                float b = Vector3DotProduct(d1, d2);
                float denom = a * e - b * b;

                if (denom > 1e-8f)
                    s = fmaxf(0.0f, fminf(1.0f, (b * f - c * e) / denom));
                else
                    s = 0.0f;

                t = (b * s + f) / e;

                if (t < 0.0f)
                {
                    t = 0.0f;
                    s = fmaxf(0.0f, fminf(1.0f, -c / a));
                }
                else if (t > 1.0f)
                {
                    t = 1.0f;
                    s = fmaxf(0.0f, fminf(1.0f, (b - c) / a));
                }
            }
        }

        c1 = Vector3Add(p1, Vector3Scale(d1, s));
        c2 = Vector3Add(p2, Vector3Scale(d2, t));

        return Vector3Distance(c1, c2);
    }

    static bool CapsuleCapsuleIntersect(const Capsule& a, const Capsule& b)
    {
        Vector3 a0, a1, b0, b1;
        CapsuleSegment(a, a0, a1);
        CapsuleSegment(b, b0, b1);

        Vector3 c1, c2;
        float dist = SegmentSegmentDistance(a0, a1, b0, b1, c1, c2);

        return dist <= a.worldRadius + b.worldRadius;
    }

    static bool SphereCapsuleIntersect(const Sphere& s, const Capsule& c)
    {
        Vector3 c0, c1;
        CapsuleSegment(c, c0, c1);

        Vector3 d = Vector3Subtract(c1, c0);
        float len2 = Vector3DotProduct(d, d);

        float t = 0.0f;

        if (len2 > 1e-8f)
        {
            t = Vector3DotProduct(Vector3Subtract(s.worldCenter, c0), d) / len2;
            t = fmaxf(0.0f, fminf(1.0f, t));
        }

        Vector3 closest = Vector3Add(c0, Vector3Scale(d, t));
        float dist = Vector3Distance(s.worldCenter, closest);

        return dist <= s.worldMaxRadius + c.worldRadius;
    }

    static bool CapsuleOBBIntersect(const Capsule& c, const OBB& o)
    {
        Vector3 c0, c1;
        CapsuleSegment(c, c0, c1);

        Quaternion invRot = QuaternionInvert(o.worldRotation);

        Vector3 l0 = Vector3Subtract(c0, o.worldCenter);
        l0 = Vector3RotateByQuaternion(l0, invRot);

        Vector3 l1 = Vector3Subtract(c1, o.worldCenter);
        l1 = Vector3RotateByQuaternion(l1, invRot);

        Vector3 d = Vector3Subtract(l1, l0);

        float tmin = 0.0f;
        float tmax = 1.0f;

        for (int axis = 0; axis < 3; axis++)
        {
            float origin = axis == 0 ? l0.x : (axis == 1 ? l0.y : l0.z);
            float direction = axis == 0 ? d.x : (axis == 1 ? d.y : d.z);
            float half = axis == 0 ? o.worldHalfExtents.x : (axis == 1 ? o.worldHalfExtents.y : o.worldHalfExtents.z);

            if (fabsf(direction) < 1e-8f)
            {
                if (origin < -half || origin > half)
                    return false;
                continue;
            }

            float invDir = 1.0f / direction;
            float t1 = (-half - origin) * invDir;
            float t2 = (half - origin) * invDir;

            if (t1 > t2)
            {
                float tmp = t1;
                t1 = t2;
                t2 = tmp;
            }

            if (t1 > tmin) tmin = t1;
            if (t2 < tmax) tmax = t2;

            if (tmin > tmax)
                return false;
        }

        float t = (tmin + tmax) * 0.5f;
        if (t < 0.0f) t = 0.0f;
        if (t > 1.0f) t = 1.0f;

        Vector3 point = Vector3Add(l0, Vector3Scale(d, t));

        Vector3 closest = {
            fmaxf(-o.worldHalfExtents.x, fminf(point.x, o.worldHalfExtents.x)),
            fmaxf(-o.worldHalfExtents.y, fminf(point.y, o.worldHalfExtents.y)),
            fmaxf(-o.worldHalfExtents.z, fminf(point.z, o.worldHalfExtents.z))
        };

        float dist = Vector3Distance(point, closest);

        return dist <= c.worldRadius;
    }

    static bool RayCapsuleIntersect(
        SGL_Ray ray,
        const Capsule& c,
        float& tHit,
        Vector3& normal)
    {
        Vector3 rayStart = { ray.startX, ray.startY, ray.startZ };
        Vector3 rayEnd = { ray.endX, ray.endY, ray.endZ };
        Vector3 rayDir = Vector3Subtract(rayEnd, rayStart);

        Vector3 c0, c1;
        CapsuleSegment(c, c0, c1);

        Vector3 axis = Vector3Subtract(c1, c0);
        float axisLen2 = Vector3DotProduct(axis, axis);

        if (axisLen2 < 1e-8f)
        {
            Sphere temp;
            temp.worldCenter = c.worldCenter;
            temp.worldRadii = { c.worldRadius, c.worldRadius, c.worldRadius };
            temp.worldMaxRadius = c.worldRadius;
            return RaySphereIntersect(ray, temp, tHit, normal);
        }

        float bestT = 1e30f;
        Vector3 bestNormal = { 0, 0, 0 };
        bool found = false;

        Vector3 axisDir = Vector3Scale(axis, 1.0f / sqrtf(axisLen2));

        Vector3 oc = Vector3Subtract(rayStart, c0);
        float ocAxis = Vector3DotProduct(oc, axisDir);
        float dirAxis = Vector3DotProduct(rayDir, axisDir);

        Vector3 ocPerp = Vector3Subtract(oc, Vector3Scale(axisDir, ocAxis));
        Vector3 dirPerp = Vector3Subtract(rayDir, Vector3Scale(axisDir, dirAxis));

        float a = Vector3DotProduct(dirPerp, dirPerp);
        float b = 2.0f * Vector3DotProduct(ocPerp, dirPerp);
        float cc = Vector3DotProduct(ocPerp, ocPerp) - c.worldRadius * c.worldRadius;

        if (fabsf(a) > 1e-8f)
        {
            float disc = b * b - 4.0f * a * cc;

            if (disc >= 0.0f)
            {
                float sqrtDisc = sqrtf(disc);
                float inv2a = 1.0f / (2.0f * a);

                float t0 = (-b - sqrtDisc) * inv2a;
                float t1 = (-b + sqrtDisc) * inv2a;

                float ts[2] = { t0, t1 };

                for (int i = 0; i < 2; i++)
                {
                    float t = ts[i];
                    if (t < 0.0f || t > 1.0f) continue;

                    Vector3 point = Vector3Add(rayStart, Vector3Scale(rayDir, t));
                    float y = Vector3DotProduct(Vector3Subtract(point, c0), axisDir);

                    if (y < 0.0f || y > sqrtf(axisLen2)) continue;

                    if (t < bestT)
                    {
                        bestT = t;
                        Vector3 n = Vector3Subtract(point, Vector3Add(c0, Vector3Scale(axisDir, y)));
                        n = Vector3Normalize(n);
                        bestNormal = n;
                        found = true;
                    }
                }
            }
        }

        float sphereTs[2] = { -1.0f, -1.0f };
        Vector3 sphereNormals[2] = { {0,0,0}, {0,0,0} };
        bool sphereHit[2] = { false, false };

        Vector3 spheres[2] = { c0, c1 };

        for (int i = 0; i < 2; i++)
        {
            Vector3 d = Vector3Subtract(rayStart, spheres[i]);

            float a2 = Vector3DotProduct(rayDir, rayDir);
            float b2 = 2.0f * Vector3DotProduct(rayDir, d);
            float c2 = Vector3DotProduct(d, d) - c.worldRadius * c.worldRadius;

            float disc = b2 * b2 - 4.0f * a2 * c2;

            if (disc < 0.0f) continue;

            float sqrtDisc = sqrtf(disc);
            float inv2a = 1.0f / (2.0f * a2);

            float t0 = (-b2 - sqrtDisc) * inv2a;
            float t1 = (-b2 + sqrtDisc) * inv2a;

            float t = t0;
            if (t < 0.0f) t = t1;
            if (t < 0.0f || t > 1.0f) continue;

            Vector3 point = Vector3Add(rayStart, Vector3Scale(rayDir, t));
            Vector3 y = Vector3Subtract(point, c0);
            float yLen = Vector3DotProduct(y, axisDir);

            if (i == 0 && yLen > 0.0f) continue;
            if (i == 1 && yLen < sqrtf(axisLen2)) continue;

            sphereHit[i] = true;
            sphereTs[i] = t;
            Vector3 n = Vector3Subtract(point, spheres[i]);
            sphereNormals[i] = Vector3Normalize(n);
        }

        for (int i = 0; i < 2; i++)
        {
            if (!sphereHit[i]) continue;
            if (sphereTs[i] < bestT)
            {
                bestT = sphereTs[i];
                bestNormal = sphereNormals[i];
                found = true;
            }
        }

        if (!found)
            return false;

        tHit = bestT;
        normal = bestNormal;

        return true;
    }

    static bool RayCylinderIntersect(
        SGL_Ray ray,
        const Cylinder& cy,
        float& tHit,
        Vector3& normal)
    {
        Vector3 rayStart = { ray.startX, ray.startY, ray.startZ };
        Vector3 rayEnd = { ray.endX, ray.endY, ray.endZ };

        Quaternion invRot = QuaternionInvert(cy.worldRotation);

        Vector3 lStart = Vector3Subtract(rayStart, cy.worldCenter);
        lStart = Vector3RotateByQuaternion(lStart, invRot);

        Vector3 lEnd = Vector3Subtract(rayEnd, cy.worldCenter);
        lEnd = Vector3RotateByQuaternion(lEnd, invRot);

        Vector3 dir = Vector3Subtract(lEnd, lStart);

        float halfH = cy.worldHeight * 0.5f;
        float R = cy.worldRadius;

        float bestT = 1e30f;
        Vector3 bestNormal = { 0, 0, 0 };
        bool found = false;

        float a = dir.x * dir.x + dir.z * dir.z;
        float b = 2.0f * (lStart.x * dir.x + lStart.z * dir.z);
        float c = lStart.x * lStart.x + lStart.z * lStart.z - R * R;

        if (fabsf(a) > 1e-8f)
        {
            float disc = b * b - 4.0f * a * c;

            if (disc >= 0.0f)
            {
                float sqrtDisc = sqrtf(disc);
                float inv2a = 1.0f / (2.0f * a);

                float t0 = (-b - sqrtDisc) * inv2a;
                float t1 = (-b + sqrtDisc) * inv2a;

                float ts[2] = { t0, t1 };

                for (int i = 0; i < 2; i++)
                {
                    float t = ts[i];
                    if (t < 0.0f || t > 1.0f) continue;

                    float y = lStart.y + dir.y * t;
                    if (y < -halfH || y > halfH) continue;

                    if (t < bestT)
                    {
                        bestT = t;
                        Vector3 p = Vector3Add(lStart, Vector3Scale(dir, t));
                        Vector3 n = { p.x, 0, p.z };
                        n = Vector3Normalize(n);
                        bestNormal = n;
                        found = true;
                    }
                }
            }
        }

        if (fabsf(dir.y) > 1e-8f)
        {
            float tTop = (halfH - lStart.y) / dir.y;
            float tBot = (-halfH - lStart.y) / dir.y;

            float ts[2] = { tTop, tBot };

            for (int i = 0; i < 2; i++)
            {
                float t = ts[i];
                if (t < 0.0f || t > 1.0f) continue;

                Vector3 p = Vector3Add(lStart, Vector3Scale(dir, t));
                if (p.x * p.x + p.z * p.z > R * R) continue;

                if (t < bestT)
                {
                    bestT = t;
                    bestNormal = { 0, (i == 0 ? 1.0f : -1.0f), 0 };
                    found = true;
                }
            }
        }

        if (!found)
            return false;

        tHit = bestT;
        normal = Vector3RotateByQuaternion(bestNormal, cy.worldRotation);

        return true;
    }

    static bool CylinderCapsuleIntersect(const Cylinder& cy, const Capsule& ca)
    {
        Capsule temp;
        temp.worldCenter = cy.worldCenter;
        temp.worldRadius = cy.worldRadius;
        temp.worldHeight = fmaxf(0.0f, cy.worldHeight - 2.0f * cy.worldRadius);
        temp.worldRotation = cy.worldRotation;

        return CapsuleCapsuleIntersect(temp, ca);
    }

    static bool CylinderCylinderIntersect(const Cylinder& a, const Cylinder& b)
    {
        Capsule tempA;
        tempA.worldCenter = a.worldCenter;
        tempA.worldRadius = a.worldRadius;
        tempA.worldHeight = fmaxf(0.0f, a.worldHeight - 2.0f * a.worldRadius);
        tempA.worldRotation = a.worldRotation;

        Capsule tempB;
        tempB.worldCenter = b.worldCenter;
        tempB.worldRadius = b.worldRadius;
        tempB.worldHeight = fmaxf(0.0f, b.worldHeight - 2.0f * b.worldRadius);
        tempB.worldRotation = b.worldRotation;

        return CapsuleCapsuleIntersect(tempA, tempB);
    }

    static bool CylinderSphereIntersect(const Cylinder& cy, const Sphere& s)
    {
        Capsule temp;
        temp.worldCenter = cy.worldCenter;
        temp.worldRadius = cy.worldRadius;
        temp.worldHeight = fmaxf(0.0f, cy.worldHeight - 2.0f * cy.worldRadius);
        temp.worldRotation = cy.worldRotation;

        return SphereCapsuleIntersect(s, temp);
    }

    static bool CylinderOBBIntersect(const Cylinder& cy, const OBB& o)
    {
        Capsule temp;
        temp.worldCenter = cy.worldCenter;
        temp.worldRadius = cy.worldRadius;
        temp.worldHeight = fmaxf(0.0f, cy.worldHeight - 2.0f * cy.worldRadius);
        temp.worldRotation = cy.worldRotation;

        return CapsuleOBBIntersect(temp, o);
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

    void PhysicalComputeSphere(const Transform& world, Sphere& sphere)
    {
        Vector3 scaledCenter = {
            sphere.localCenter.x * world.scale.x,
            sphere.localCenter.y * world.scale.y,
            sphere.localCenter.z * world.scale.z
        };

        Vector3 rotatedCenter = Vector3RotateByQuaternion(scaledCenter, world.rotation);
        sphere.worldCenter = Vector3Add(world.position, rotatedCenter);

        sphere.worldRadii = {
            sphere.localRadius * world.scale.x,
            sphere.localRadius * world.scale.y,
            sphere.localRadius * world.scale.z
        };

        float maxScale = fmaxf(world.scale.x, fmaxf(world.scale.y, world.scale.z));
        sphere.worldMaxRadius = sphere.localRadius * maxScale;
    }

    void PhysicalComputeCapsule(const Transform& world, Capsule& capsule)
    {
        Vector3 scaledCenter = {
            capsule.localCenter.x * world.scale.x,
            capsule.localCenter.y * world.scale.y,
            capsule.localCenter.z * world.scale.z
        };

        Vector3 rotatedCenter = Vector3RotateByQuaternion(scaledCenter, world.rotation);
        capsule.worldCenter = Vector3Add(world.position, rotatedCenter);

        float maxRadialScale = fmaxf(world.scale.x, world.scale.z);

        capsule.worldRadius = capsule.localRadius * maxRadialScale;
        capsule.worldHeight = capsule.localHeight * world.scale.y;
        capsule.worldRotation = world.rotation;
    }

    void PhysicalComputeCylinder(const Transform& world, Cylinder& cylinder)
    {
        Vector3 scaledCenter = {
            cylinder.localCenter.x * world.scale.x,
            cylinder.localCenter.y * world.scale.y,
            cylinder.localCenter.z * world.scale.z
        };

        Vector3 rotatedCenter = Vector3RotateByQuaternion(scaledCenter, world.rotation);
        cylinder.worldCenter = Vector3Add(world.position, rotatedCenter);

        float maxRadialScale = fmaxf(world.scale.x, world.scale.z);

        cylinder.worldRadius = cylinder.localRadius * maxRadialScale;
        cylinder.worldHeight = cylinder.localHeight * world.scale.y;
        cylinder.worldRotation = world.rotation;
    }

    int PhysicalCheckCollision(int a, int b)
    {
        Scene& scene = GetSceneInstance();

        if (!scene.IsAlive(a) || !scene.IsAlive(b)) return 0;

        if (!scene.HasComponent<Physical>(a)) return 0;
        if (!scene.HasComponent<Physical>(b)) return 0;

        if (scene.GetComponent<Physical>(a).canCollide == 0) return 0;
        if (scene.GetComponent<Physical>(b).canCollide == 0) return 0;

        bool hasObbA = scene.HasComponent<OBB>(a);
        bool hasObbB = scene.HasComponent<OBB>(b);
        bool hasSphereA = scene.HasComponent<Sphere>(a);
        bool hasSphereB = scene.HasComponent<Sphere>(b);
        bool hasCapsuleA = scene.HasComponent<Capsule>(a);
        bool hasCapsuleB = scene.HasComponent<Capsule>(b);
        bool hasCylinderA = scene.HasComponent<Cylinder>(a);
        bool hasCylinderB = scene.HasComponent<Cylinder>(b);

        if (hasSphereA && hasSphereB)
        {
            return SphereSphereIntersect(scene.GetComponent<Sphere>(a), scene.GetComponent<Sphere>(b)) ? 1 : 0;
        }

        if (hasSphereA && hasObbB)
        {
            return SphereOBBIntersect(scene.GetComponent<Sphere>(a), scene.GetComponent<OBB>(b)) ? 1 : 0;
        }

        if (hasObbA && hasSphereB)
        {
            return SphereOBBIntersect(scene.GetComponent<Sphere>(b), scene.GetComponent<OBB>(a)) ? 1 : 0;
        }

        if (hasObbA && hasObbB)
        {
            return OBBIntersect(scene.GetComponent<OBB>(a), scene.GetComponent<OBB>(b)) ? 1 : 0;
        }

        if (hasCapsuleA && hasCapsuleB)
        {
            return CapsuleCapsuleIntersect(scene.GetComponent<Capsule>(a), scene.GetComponent<Capsule>(b)) ? 1 : 0;
        }

        if (hasCapsuleA && hasSphereB)
        {
            return SphereCapsuleIntersect(scene.GetComponent<Sphere>(b), scene.GetComponent<Capsule>(a)) ? 1 : 0;
        }

        if (hasSphereA && hasCapsuleB)
        {
            return SphereCapsuleIntersect(scene.GetComponent<Sphere>(a), scene.GetComponent<Capsule>(b)) ? 1 : 0;
        }

        if (hasCapsuleA && hasObbB)
        {
            return CapsuleOBBIntersect(scene.GetComponent<Capsule>(a), scene.GetComponent<OBB>(b)) ? 1 : 0;
        }

        if (hasObbA && hasCapsuleB)
        {
            return CapsuleOBBIntersect(scene.GetComponent<Capsule>(b), scene.GetComponent<OBB>(a)) ? 1 : 0;
        }

        if (hasCylinderA && hasCylinderB)
        {
            return CylinderCylinderIntersect(scene.GetComponent<Cylinder>(a), scene.GetComponent<Cylinder>(b)) ? 1 : 0;
        }

        if (hasCylinderA && hasCapsuleB)
        {
            return CylinderCapsuleIntersect(scene.GetComponent<Cylinder>(a), scene.GetComponent<Capsule>(b)) ? 1 : 0;
        }

        if (hasCapsuleA && hasCylinderB)
        {
            return CylinderCapsuleIntersect(scene.GetComponent<Cylinder>(b), scene.GetComponent<Capsule>(a)) ? 1 : 0;
        }

        if (hasCylinderA && hasSphereB)
        {
            return CylinderSphereIntersect(scene.GetComponent<Cylinder>(a), scene.GetComponent<Sphere>(b)) ? 1 : 0;
        }

        if (hasSphereA && hasCylinderB)
        {
            return CylinderSphereIntersect(scene.GetComponent<Cylinder>(b), scene.GetComponent<Sphere>(a)) ? 1 : 0;
        }

        if (hasCylinderA && hasObbB)
        {
            return CylinderOBBIntersect(scene.GetComponent<Cylinder>(a), scene.GetComponent<OBB>(b)) ? 1 : 0;
        }

        if (hasObbA && hasCylinderB)
        {
            return CylinderOBBIntersect(scene.GetComponent<Cylinder>(b), scene.GetComponent<OBB>(a)) ? 1 : 0;
        }

        return 0;
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
            if (!scene.HasComponent<Physical>(i)) continue;
            if (scene.GetComponent<Physical>(i).canCollide == 0) continue;

            float tHit = 0.0f;
            Vector3 normal = { 0, 0, 0 };

            if (scene.HasComponent<Sphere>(i))
            {
                if (!RaySphereIntersect(ray, scene.GetComponent<Sphere>(i), tHit, normal)) continue;
            }
            else if (scene.HasComponent<Capsule>(i))
            {
                if (!RayCapsuleIntersect(ray, scene.GetComponent<Capsule>(i), tHit, normal)) continue;
            }
            else if (scene.HasComponent<Cylinder>(i))
            {
                if (!RayCylinderIntersect(ray, scene.GetComponent<Cylinder>(i), tHit, normal)) continue;
            }
            else if (scene.HasComponent<OBB>(i))
            {
                if (!RayOBBIntersect(ray, scene.GetComponent<OBB>(i), tHit, normal)) continue;
            }
            else
            {
                continue;
            }

            if (tHit >= closest) continue;

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