#include "capsule.h"

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
        meshID.value = 2;
        scene.AddComponent<MeshID>(handle, meshID);

        OBB box;
        box.localCenter = { 0, 0, 0 };
        box.localHalfExtents = { 0.5f, 1.0f, 0.5f };
        box.localRotation = QuaternionIdentity();
        scene.AddComponent<OBB>(handle, box);

        Capsule capsule;
        capsule.localCenter = { 0, 0, 0 };
        capsule.localRadius = 0.5f;
        capsule.localHeight = 1.0f;
        scene.AddComponent<Capsule>(handle, capsule);

        scene.UpdateEntityTree(handle);

        return handle;
    }

}