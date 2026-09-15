#include "cube.h"
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

    int CreateCube()
    {
        Scene& scene = GetSceneInstance();

        int handle = scene.CreateEntity();

        scene.AddComponent<Transform>(handle, Transform());
        scene.AddComponent<Color>(handle, Color());
        scene.AddComponent<Visible>(handle, Visible());
        scene.AddComponent<Physical>(handle, Physical());

        MeshID meshID;
        meshID.value = GetBuiltinCubeMeshHandle();
        scene.AddComponent<MeshID>(handle, meshID);

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