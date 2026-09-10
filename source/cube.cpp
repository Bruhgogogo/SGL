#include "cube.h"

namespace SGL
{

    int CreateCube()
    {
        int handle = CreateEntity();

        AddComponent<Transform>(handle, Transform());
        AddComponent<Color>(handle, Color());
        AddComponent<Visible>(handle, Visible());

        return handle;
    }

}