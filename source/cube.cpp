#include "cube.h"
#include "physical.h"

// ====================================================================================================
// NAMESPACE
// ====================================================================================================

namespace SGL
{

    // ====================================================================================================
    // APIs
    // ====================================================================================================

    int CreateCube()
    {
        Scene& scene = GetSceneInstance();

        int handle = scene.CreateEntity();

        scene.AddComponent<Transform>(handle, Transform());
        scene.AddComponent<Color>(handle, Color());
        scene.AddComponent<Visible>(handle, Visible());
        scene.AddComponent<Physical>(handle, Physical());
        scene.AddComponent<MeshID>(handle, MeshID());

        OBB box;
        box.localCenter = { 0, 0, 0 };
        box.localHalfExtents = { 0.5f, 0.5f, 0.5f };
        box.localRotation = QuaternionIdentity();

        scene.AddComponent<OBB>(handle, box);

        scene.UpdateEntityTree(handle);

        GetJoltWorldInstance().CreateBody(
            handle,
            scene.GetWorldTransform(handle),
            scene.GetComponent<Physical>(handle),
            &scene.GetComponent<OBB>(handle),
            nullptr,
            nullptr,
            nullptr
        );

        return handle;
    }

}