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
        scene.AddComponent<MeshID>(handle, MeshID());

        return handle;
    }

}