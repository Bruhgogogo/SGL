#include "meshEntity.h"
#include "mesh.h"
#include "physical.h"

// ====================================================================================================
// NAMESPACE
// ====================================================================================================

namespace SGL
{

    // ====================================================================================================
    // APIs
    // ====================================================================================================

    int CreateMesh(int meshHandle)
    {
        Scene& scene = GetSceneInstance();

        if (!IsMeshValid(meshHandle)) return -1;

        int handle = scene.CreateEntity();

        scene.AddComponent<Transform>(handle, Transform());
        scene.AddComponent<Color>(handle, Color());
        scene.AddComponent<Visible>(handle, Visible());
        scene.AddComponent<Physical>(handle, Physical());

        MeshID meshID;
        meshID.value = meshHandle;
        scene.AddComponent<MeshID>(handle, meshID);

        MeshShape meshShape;

        int partCount = GetMeshPartCount(meshHandle);
        meshShape.partCount = partCount;

        for (int i = 0; i < partCount; i++)
        {
            const MeshPart& part = GetMeshPart(meshHandle, i);
            meshShape.localMin[i] = part.localMin;
            meshShape.localMax[i] = part.localMax;
        }

        scene.AddComponent<MeshShape>(handle, meshShape);

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