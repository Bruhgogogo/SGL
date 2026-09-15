#include "cylinder.h"
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

    int CreateCylinder()
    {
        Scene& scene = GetSceneInstance();

        int handle = scene.CreateEntity();

        scene.AddComponent<Transform>(handle, Transform());
        scene.AddComponent<Color>(handle, Color());
        scene.AddComponent<Visible>(handle, Visible());
        scene.AddComponent<Physical>(handle, Physical());

        MeshID meshID;
        meshID.value = GetBuiltinCylinderMeshHandle();
        scene.AddComponent<MeshID>(handle, meshID);

        CylinderShape cylinder;
        cylinder.localRadius = 0.5f;
        cylinder.localHeight = 1.0f;
        scene.AddComponent<CylinderShape>(handle, cylinder);

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