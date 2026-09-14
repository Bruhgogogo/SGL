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

        void CreateBody(int handle, const Transform& world, const Physical& physical);
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

        void SetGravity(float x, float y, float z);
        void GetGravity(float* x, float* y, float* z);

        SGL_RayHit Raycast(SGL_Ray ray);

        void SyncFromECS(int handle, const Transform& world);
        void SyncToECS(int handle, Transform& world);

        void Update(float deltaTime, int steps);

        void PhysicalUpdateWorld(float deltaTime, int steps);
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

    void ShapeUpdateWorld(int handle, const Transform& world);

    SGL_RayHit PhysicalRaycast(SGL_Ray ray);

    JoltWorld& GetJoltWorldInstance();
}