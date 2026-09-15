#include "capsule.h"
#include "physical.h"
#include "graphics.h"

// ====================================================================================================
// NAMESPACE
// ====================================================================================================

namespace SGL
{

    // ====================================================================================================
    // APIs
    // ====================================================================================================

    int CreateCapsule()
    {
        Scene& scene = GetSceneInstance();

        int handle = scene.CreateEntity();

        scene.AddComponent<Transform>(handle, Transform());
        scene.AddComponent<Color>(handle, Color());
        scene.AddComponent<Visible>(handle, Visible());
        scene.AddComponent<Physical>(handle, Physical());

        MeshID meshID;
        meshID.value = GetBuiltinCapsuleMeshHandle();
        scene.AddComponent<MeshID>(handle, meshID);

        CapsuleShape capsule;
        capsule.localRadius = 0.5f;
        capsule.localHeight = 1.0f;
        scene.AddComponent<CapsuleShape>(handle, capsule);

        scene.AddComponent<AABB>(handle, AABB());

        scene.UpdateEntityTree(handle);

        GetJoltWorldInstance().CreateBody(
            handle,
            scene.GetWorldTransform(handle),
            scene.GetComponent<Physical>(handle)
        );

        return handle;
    }

}