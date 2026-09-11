#include "cylinder.h"

// ====================================================================================================
// NAMESPACE
// ====================================================================================================

namespace SGL
{

    // ====================================================================================================
    // APIs
    // ====================================================================================================

    int CreateCylinder()
    {
        Scene& scene = GetSceneInstance();

        int handle = scene.CreateEntity();

        scene.AddComponent<Transform>(handle, Transform());
        scene.AddComponent<Color>(handle, Color());
        scene.AddComponent<Visible>(handle, Visible());
        scene.AddComponent<Physical>(handle, Physical());

        MeshID meshID;
        meshID.value = 3;
        scene.AddComponent<MeshID>(handle, meshID);

        OBB box;
        box.localCenter = { 0, 0, 0 };
        box.localHalfExtents = { 0.5f, 0.5f, 0.5f };
        box.localRotation = QuaternionIdentity();
        scene.AddComponent<OBB>(handle, box);

        Cylinder cylinder;
        cylinder.localCenter = { 0, 0, 0 };
        cylinder.localRadius = 0.5f;
        cylinder.localHeight = 1.0f;
        scene.AddComponent<Cylinder>(handle, cylinder);

        scene.UpdateEntityTree(handle);

        return handle;
    }

}