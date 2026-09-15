#include "mesh.h"

#include <vector>

// ====================================================================================================
// NAMESPACE
// ====================================================================================================

namespace SGL
{
    // ====================================================================================================
    // STATIC
    // ====================================================================================================

    static std::vector<Model> Meshes;
    static std::vector<int>   FreeNext;
    static int                FreeList = -1;

    // ====================================================================================================
    // APIs
    // ====================================================================================================

    int RegisterModel(const Model& model)
    {
        int handle;

        if (FreeList != -1)
        {
            handle = FreeList;
            FreeList = FreeNext[handle];
        }
        else
        {
            handle = (int)Meshes.size();
            Meshes.push_back(Model());
            FreeNext.push_back(-1);
        }

        Meshes[handle] = model;
        FreeNext[handle] = -1;

        return handle;
    }

    int RegisterMesh(const Mesh& mesh)
    {
        Model model = LoadModelFromMesh(mesh);
        return RegisterModel(model);
    }

    void UnregisterModel(int handle)
    {
        if (handle < 0) return;
        if (handle >= (int)Meshes.size()) return;
        if (Meshes[handle].meshCount == 0) return;

        UnloadModel(Meshes[handle]);
        Meshes[handle] = Model();

        FreeNext[handle] = FreeList;
        FreeList = handle;
    }

    Model& GetMeshModel(int handle)
    {
        return Meshes[handle];
    }

    bool IsMeshValid(int handle)
    {
        if (handle < 0) return false;
        if (handle >= (int)Meshes.size()) return false;
        if (Meshes[handle].meshCount == 0) return false;
        return true;
    }

    void MeshShutdown()
    {
        for (int i = 0; i < (int)Meshes.size(); i++)
        {
            if (Meshes[i].meshCount == 0) continue;

            UnloadModel(Meshes[i]);
            Meshes[i] = Model();
        }

        Meshes.clear();
        FreeNext.clear();
        FreeList = -1;
    }

}