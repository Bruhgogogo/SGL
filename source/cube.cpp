#include "cube.h"

namespace SGL
{

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
        box.center = { 0, 0, 0 };
        box.halfExtents = { 0.5f, 0.5f, 0.5f };
        box.rotation = QuaternionIdentity();

        scene.AddComponent<OBB>(handle, box);

        scene.UpdateEntityTree(handle);

        return handle;
    }

}