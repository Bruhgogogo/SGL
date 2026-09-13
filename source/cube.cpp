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

        BoxShape box;
        box.localHalfExtents = { 0.5f, 0.5f, 0.5f };
        scene.AddComponent<BoxShape>(handle, box);

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