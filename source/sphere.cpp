#include "sphere.h"
#include "physical.h"

// ====================================================================================================
// NAMESPACE
// ====================================================================================================

namespace SGL
{

    // ====================================================================================================
    // APIs
    // ====================================================================================================

    int CreateSphere()
    {
        Scene& scene = GetSceneInstance();

        int handle = scene.CreateEntity();

        scene.AddComponent<Transform>(handle, Transform());
        scene.AddComponent<Color>(handle, Color());
        scene.AddComponent<Visible>(handle, Visible());
        scene.AddComponent<Physical>(handle, Physical());

        MeshID meshID;
        meshID.value = 1;
        scene.AddComponent<MeshID>(handle, meshID);

        SphereShape sphere;
        sphere.localRadius = 0.5f;
        scene.AddComponent<SphereShape>(handle, sphere);

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