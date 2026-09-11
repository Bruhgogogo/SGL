#ifdef _WIN32

#ifdef _DEBUG
#pragma comment(lib, "Jolt_Debug.lib")
#else
#pragma comment(lib, "Jolt_Release.lib")
#endif

#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "gdi32.lib")

#endif

#include "physical.h"
#include "scene.h"

#include <Jolt/Jolt.h>
#include <Jolt/RegisterTypes.h>
#include <Jolt/Core/Factory.h>
#include <Jolt/Core/TempAllocator.h>
#include <Jolt/Core/JobSystemThreadPool.h>
#include <Jolt/Physics/PhysicsSettings.h>
#include <Jolt/Physics/Collision/Shape/BoxShape.h>
#include <Jolt/Physics/Collision/Shape/SphereShape.h>
#include <Jolt/Physics/Collision/Shape/CapsuleShape.h>
#include <Jolt/Physics/Collision/Shape/CylinderShape.h>
#include <Jolt/Physics/Collision/RayCast.h>
#include <Jolt/Physics/Collision/CastResult.h>
#include <Jolt/Physics/Body/BodyCreationSettings.h>

// ====================================================================================================
// NAMESPACE
// ====================================================================================================

namespace SGL
{
    // ====================================================================================================
    // LAYERS
    // ====================================================================================================

    namespace ObjectLayers
    {
        static constexpr JPH::ObjectLayer NON_MOVING = 0;
        static constexpr JPH::ObjectLayer MOVING = 1;
        static constexpr JPH::ObjectLayer NUM_LAYERS = 2;
    }

    namespace BroadPhaseLayers
    {
        static constexpr JPH::BroadPhaseLayer NON_MOVING(0);
        static constexpr JPH::BroadPhaseLayer MOVING(1);
        static constexpr JPH::uint NUM_LAYERS = 2;
    }

    class BPLayerInterfaceImpl final : public JPH::BroadPhaseLayerInterface
    {
    public:
        BPLayerInterfaceImpl()
        {
            objectToBroadPhase[ObjectLayers::NON_MOVING] = BroadPhaseLayers::NON_MOVING;
            objectToBroadPhase[ObjectLayers::MOVING] = BroadPhaseLayers::MOVING;
        }

        virtual JPH::uint GetNumBroadPhaseLayers() const override
        {
            return BroadPhaseLayers::NUM_LAYERS;
        }

        virtual JPH::BroadPhaseLayer GetBroadPhaseLayer(JPH::ObjectLayer layer) const override
        {
            return objectToBroadPhase[layer];
        }

    private:
        JPH::BroadPhaseLayer objectToBroadPhase[ObjectLayers::NUM_LAYERS];
    };

    class ObjectVsBroadPhaseLayerFilterImpl : public JPH::ObjectVsBroadPhaseLayerFilter
    {
    public:
        virtual bool ShouldCollide(JPH::ObjectLayer layer1, JPH::BroadPhaseLayer layer2) const override
        {
            if (layer1 == ObjectLayers::NON_MOVING)
                return layer2 == BroadPhaseLayers::MOVING;
            return true;
        }
    };

    class ObjectLayerPairFilterImpl : public JPH::ObjectLayerPairFilter
    {
    public:
        virtual bool ShouldCollide(JPH::ObjectLayer layer1, JPH::ObjectLayer layer2) const override
        {
            if (layer1 == ObjectLayers::NON_MOVING && layer2 == ObjectLayers::NON_MOVING)
                return false;
            return true;
        }
    };

    // ====================================================================================================
    // STATIC
    // ====================================================================================================

    static JoltWorld JoltWorldInstance;

    static BPLayerInterfaceImpl broadPhaseLayerInterface;
    static ObjectVsBroadPhaseLayerFilterImpl objectVsBroadPhaseLayerFilter;
    static ObjectLayerPairFilterImpl objectLayerPairFilter;

    static JPH::TempAllocatorImpl* tempAllocator = nullptr;
    static JPH::JobSystemThreadPool* jobSystem = nullptr;

    static void SGLTraceImpl(const char* inFMT, ...)
    {
        va_list list;
        va_start(list, inFMT);
        char buffer[1024];
        vsnprintf(buffer, sizeof(buffer), inFMT, list);
        va_end(list);
        printf("[Jolt] %s\n", buffer);
    }

    static bool SGLAssertFailedImpl(const char* inExpression, const char* inMessage, const char* inFile, unsigned int inLine)
    {
        printf("[Jolt Assert] %s:%u: (%s) %s\n", inFile, inLine, inExpression, inMessage ? inMessage : "");
        return true;
    }

    // ====================================================================================================
    // JOLT WORLD
    // ====================================================================================================

    JoltWorld& GetJoltWorldInstance()
    {
        return JoltWorldInstance;
    }

    void JoltWorld::Init()
    {
        if (JPH::Factory::sInstance != nullptr) return;

        JPH::Trace = SGLTraceImpl;
        JPH_IF_ENABLE_ASSERTS(JPH::AssertFailed = SGLAssertFailedImpl;)

        JPH::RegisterDefaultAllocator();
        JPH::Factory::sInstance = new JPH::Factory();
        JPH::RegisterTypes();

        const JPH::uint cMaxBodies = 65536;
        const JPH::uint cNumBodyMutexes = 0;
        const JPH::uint cMaxBodyPairs = 65536;
        const JPH::uint cMaxContactConstraints = 10240;

        physicsSystem.Init(
            cMaxBodies,
            cNumBodyMutexes,
            cMaxBodyPairs,
            cMaxContactConstraints,
            broadPhaseLayerInterface,
            objectVsBroadPhaseLayerFilter,
            objectLayerPairFilter
        );

        physicsSystem.SetGravity(JPH::Vec3(0, -9.81f, 0));

        bodyInterface = &physicsSystem.GetBodyInterface();

        tempAllocator = new JPH::TempAllocatorImpl(10 * 1024 * 1024);
        jobSystem = new JPH::JobSystemThreadPool(JPH::cMaxPhysicsJobs, JPH::cMaxPhysicsBarriers, 4);

        bodyIDs.resize(1024);
    }

    void JoltWorld::Shutdown()
    {
        if (bodyInterface != nullptr)
        {
            for (int i = 0; i < (int)bodyIDs.size(); i++)
            {
                if (bodyIDs[i].IsInvalid()) continue;

                bodyInterface->RemoveBody(bodyIDs[i]);
                bodyInterface->DestroyBody(bodyIDs[i]);
            }

            bodyIDs.clear();
            bodyInterface = nullptr;
        }

        delete tempAllocator;
        delete jobSystem;

        tempAllocator = nullptr;
        jobSystem = nullptr;

        if (JPH::Factory::sInstance != nullptr)
        {
            JPH::UnregisterTypes();
            delete JPH::Factory::sInstance;
            JPH::Factory::sInstance = nullptr;
        }
    }

    JPH::BodyID JoltWorld::GetBodyID(int handle) const
    {
        if (handle < 0) return JPH::BodyID();
        if (handle >= (int)bodyIDs.size()) return JPH::BodyID();
        return bodyIDs[handle];
    }

    void JoltWorld::CreateBody(int handle, const Transform& world, const Physical& physical,
        const OBB* obb, const Sphere* sphere,
        const Capsule* capsule, const Cylinder* cylinder)
    {
        if (handle < 0) return;

        if (handle >= (int)bodyIDs.size())
            bodyIDs.resize(handle + 1);

        if (!bodyIDs[handle].IsInvalid()) return;

        if (physical.canCollide == 0) return;

        JPH::ShapeSettings* shapeSettings = nullptr;

        if (sphere)
        {
            shapeSettings = new JPH::SphereShapeSettings(sphere->worldMaxRadius);
        }
        else if (capsule)
        {
            float halfHeight = capsule->worldHeight * 0.5f;
            shapeSettings = new JPH::CapsuleShapeSettings(halfHeight, capsule->worldRadius);
        }
        else if (cylinder)
        {
            float halfHeight = cylinder->worldHeight * 0.5f;
            shapeSettings = new JPH::CylinderShapeSettings(halfHeight, cylinder->worldRadius);
        }
        else if (obb)
        {
            JPH::Vec3 halfExtents(
                obb->worldHalfExtents.x,
                obb->worldHalfExtents.y,
                obb->worldHalfExtents.z);

            shapeSettings = new JPH::BoxShapeSettings(halfExtents);
        }

        if (!shapeSettings) return;

        JPH::ShapeSettings::ShapeResult shapeResult = shapeSettings->Create();
        delete shapeSettings;

        if (shapeResult.HasError()) return;

        JPH::BodyCreationSettings bodySettings(
            shapeResult.Get(),
            JPH::RVec3(world.position.x, world.position.y, world.position.z),
            JPH::Quat(world.rotation.x, world.rotation.y, world.rotation.z, world.rotation.w),
            physical.anchored ? JPH::EMotionType::Kinematic : JPH::EMotionType::Dynamic,
            physical.anchored ? ObjectLayers::NON_MOVING : ObjectLayers::MOVING
        );

        bodySettings.mFriction = physical.friction;
        bodySettings.mRestitution = physical.restitution;
        bodySettings.mLinearDamping = physical.linearDamping;
        bodySettings.mAngularDamping = physical.angularDamping;
        bodySettings.mGravityFactor = physical.gravityFactor;
        bodySettings.mAllowSleeping = physical.allowSleeping != 0;
        bodySettings.mIsSensor = physical.isSensor != 0;

        if (!physical.anchored)
            bodySettings.mMassPropertiesOverride.mMass = physical.mass;

        JPH::Body* body = bodyInterface->CreateBody(bodySettings);

        if (!body) return;

        bodyInterface->AddBody(body->GetID(), JPH::EActivation::Activate);

        bodyIDs[handle] = body->GetID();
    }

    void JoltWorld::DestroyBody(int handle)
    {
        if (handle < 0) return;
        if (handle >= (int)bodyIDs.size()) return;

        JPH::BodyID id = bodyIDs[handle];

        if (id.IsInvalid()) return;

        bodyInterface->RemoveBody(id);
        bodyInterface->DestroyBody(id);

        bodyIDs[handle] = JPH::BodyID();
    }

    void JoltWorld::SetBodyVelocity(int handle, float vx, float vy, float vz)
    {
        JPH::BodyID id = GetBodyID(handle);

        if (id.IsInvalid()) return;

        bodyInterface->SetLinearVelocity(id, JPH::Vec3(vx, vy, vz));
        bodyInterface->ActivateBody(id);
    }

    void JoltWorld::GetBodyVelocity(int handle, float* vx, float* vy, float* vz)
    {
        JPH::BodyID id = GetBodyID(handle);

        if (id.IsInvalid())
        {
            *vx = 0.0f;
            *vy = 0.0f;
            *vz = 0.0f;
            return;
        }

        JPH::Vec3 velocity = bodyInterface->GetLinearVelocity(id);

        *vx = velocity.GetX();
        *vy = velocity.GetY();
        *vz = velocity.GetZ();
    }

    void JoltWorld::ApplyBodyImpulse(int handle, float fx, float fy, float fz)
    {
        JPH::BodyID id = GetBodyID(handle);

        if (id.IsInvalid()) return;

        bodyInterface->AddImpulse(id, JPH::Vec3(fx, fy, fz));
        bodyInterface->ActivateBody(id);
    }

    void JoltWorld::SetBodyAngularVelocity(int handle, float wx, float wy, float wz)
    {
        JPH::BodyID id = GetBodyID(handle);

        if (id.IsInvalid()) return;

        bodyInterface->SetAngularVelocity(id, JPH::Vec3(wx, wy, wz));
        bodyInterface->ActivateBody(id);
    }

    void JoltWorld::GetBodyAngularVelocity(int handle, float* wx, float* wy, float* wz)
    {
        JPH::BodyID id = GetBodyID(handle);

        if (id.IsInvalid())
        {
            *wx = 0.0f;
            *wy = 0.0f;
            *wz = 0.0f;
            return;
        }

        JPH::Vec3 velocity = bodyInterface->GetAngularVelocity(id);

        *wx = velocity.GetX();
        *wy = velocity.GetY();
        *wz = velocity.GetZ();
    }

    void JoltWorld::ApplyBodyAngularImpulse(int handle, float wx, float wy, float wz)
    {
        JPH::BodyID id = GetBodyID(handle);

        if (id.IsInvalid()) return;

        bodyInterface->AddAngularImpulse(id, JPH::Vec3(wx, wy, wz));
        bodyInterface->ActivateBody(id);
    }

    void JoltWorld::TeleportBody(int handle, float x, float y, float z)
    {
        JPH::BodyID id = GetBodyID(handle);

        if (id.IsInvalid()) return;

        bodyInterface->SetPosition(id, JPH::RVec3(x, y, z), JPH::EActivation::Activate);
        bodyInterface->SetLinearVelocity(id, JPH::Vec3::sZero());
        bodyInterface->SetAngularVelocity(id, JPH::Vec3::sZero());
    }

    void JoltWorld::SyncFromECS(int handle, const Transform& world)
    {
        JPH::BodyID id = GetBodyID(handle);

        if (id.IsInvalid()) return;

        bodyInterface->SetPositionAndRotation(
            id,
            JPH::RVec3(world.position.x, world.position.y, world.position.z),
            JPH::Quat(world.rotation.x, world.rotation.y, world.rotation.z, world.rotation.w),
            JPH::EActivation::Activate
        );
    }

    void JoltWorld::SyncToECS(int handle, Transform& world)
    {
        JPH::BodyID id = GetBodyID(handle);

        if (id.IsInvalid()) return;

        JPH::RVec3 position = bodyInterface->GetPosition(id);
        JPH::Quat rotation = bodyInterface->GetRotation(id);

        world.position = {
            (float)position.GetX(),
            (float)position.GetY(),
            (float)position.GetZ()
        };

        world.rotation = {
            rotation.GetX(),
            rotation.GetY(),
            rotation.GetZ(),
            rotation.GetW()
        };
    }

    SGL_RayHit JoltWorld::Raycast(SGL_Ray ray)
    {
        SGL_RayHit result = {};
        result.entity = -1;

        JPH::RRayCast rayCast(
            JPH::RVec3(ray.startX, ray.startY, ray.startZ),
            JPH::Vec3(
                ray.endX - ray.startX,
                ray.endY - ray.startY,
                ray.endZ - ray.startZ
            )
        );

        JPH::RayCastResult hit;

        if (physicsSystem.GetNarrowPhaseQuery().CastRay(rayCast, hit))
        {
            JPH::BodyID bodyID = hit.mBodyID;

            for (int i = 0; i < (int)bodyIDs.size(); i++)
            {
                if (bodyIDs[i] == bodyID)
                {
                    result.hit = 1;
                    result.entity = i;

                    JPH::RVec3 point = rayCast.GetPointOnRay(hit.mFraction);

                    result.pointX = (float)point.GetX();
                    result.pointY = (float)point.GetY();
                    result.pointZ = (float)point.GetZ();
                    result.distance = (float)(hit.mFraction * rayCast.mDirection.Length());

                    break;
                }
            }
        }

        return result;
    }

    void JoltWorld::Update(float deltaTime)
    {
        if (!tempAllocator || !jobSystem) return;

        physicsSystem.Update(
            deltaTime,
            1,
            tempAllocator,
            jobSystem
        );
    }

    void JoltWorld::SetBodyAnchored(int handle, bool anchored)
    {
        JPH::BodyID id = GetBodyID(handle);
        if (id.IsInvalid()) return;

        JPH::EMotionType type = anchored ? JPH::EMotionType::Kinematic : JPH::EMotionType::Dynamic;
        JPH::ObjectLayer layer = anchored ? ObjectLayers::NON_MOVING : ObjectLayers::MOVING;

        bodyInterface->SetMotionType(id, type, JPH::EActivation::Activate);
        bodyInterface->SetObjectLayer(id, layer);
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

        capsule.worldRadius = capsule.localRadius * world.scale.x;
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

        cylinder.worldRadius = cylinder.localRadius * world.scale.x;
        cylinder.worldHeight = cylinder.localHeight * world.scale.y;
        cylinder.worldRotation = world.rotation;
    }

    SGL_RayHit PhysicalRaycast(SGL_Ray ray)
    {
        return GetJoltWorldInstance().Raycast(ray);
    }

}