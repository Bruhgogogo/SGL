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

        OBB box;
        box.localCenter = { 0, 0, 0 };
        box.localHalfExtents = { 0.5f, 0.5f, 0.5f };
        box.localRotation = QuaternionIdentity();
        scene.AddComponent<OBB>(handle, box);

        Sphere sphere;
        sphere.localCenter = { 0, 0, 0 };
        sphere.localRadius = 0.5f;
        scene.AddComponent<Sphere>(handle, sphere);

        scene.UpdateEntityTree(handle);

        GetJoltWorldInstance().CreateBody(
            handle,
            scene.GetWorldTransform(handle),
            scene.GetComponent<Physical>(handle),
            nullptr,
            &scene.GetComponent<Sphere>(handle),
            nullptr,
            nullptr
        );

        return handle;
    }

}