#pragma once

#include "raylibUtils.h"
#include "component.h"
#include "SGL.h"

#include <Jolt/Jolt.h>
#include <Jolt/Physics/PhysicsSystem.h>
#include <Jolt/Physics/Body/BodyInterface.h>
#include <Jolt/Physics/Body/BodyID.h>

#include <vector>

// ====================================================================================================
// NAMESPACE
// ====================================================================================================

namespace SGL
{
    // ====================================================================================================
    // JOLT WORLD
    // ====================================================================================================

    class JoltWorld
    {
    public:
        void Init();
        void Shutdown();

        void CreateBody(int handle, const Transform& world, const Physical& physical,
            const OBB* obb, const Sphere* sphere,
            const Capsule* capsule, const Cylinder* cylinder);
        void DestroyBody(int handle);

        void SetBodyVelocity(int handle, float vx, float vy, float vz);
        void GetBodyVelocity(int handle, float* vx, float* vy, float* vz);
        void ApplyBodyImpulse(int handle, float fx, float fy, float fz);
        void SetBodyAngularVelocity(int handle, float wx, float wy, float wz);
        void GetBodyAngularVelocity(int handle, float* wx, float* wy, float* wz);
        void ApplyBodyAngularImpulse(int handle, float wx, float wy, float wz);
        void TeleportBody(int handle, float x, float y, float z);
        void SetBodyCollide(int handle, bool collide);
        void SetBodyAnchored(int handle, bool anchored);

        SGL_RayHit Raycast(SGL_Ray ray);

        void SyncFromECS(int handle, const Transform& world);
        void SyncToECS(int handle, Transform& world);

        void Update(float deltaTime);

        void PhysicalUpdateWorld(float deltaTime);
        void PhysicalRebuildBody(int handle);

    private:
        JPH::PhysicsSystem   physicsSystem;
        JPH::BodyInterface* bodyInterface = nullptr;

        std::vector<JPH::BodyID> bodyIDs;

        JPH::BodyID GetBodyID(int handle) const;
    };

    // ====================================================================================================
    // APIs
    // ====================================================================================================

    void PhysicalComputeOBB(const Transform& world, OBB& obb);
    void PhysicalComputeSphere(const Transform& world, Sphere& sphere);
    void PhysicalComputeCapsule(const Transform& world, Capsule& capsule);
    void PhysicalComputeCylinder(const Transform& world, Cylinder& cylinder);

    SGL_RayHit PhysicalRaycast(SGL_Ray ray);

    JoltWorld& GetJoltWorldInstance();

}