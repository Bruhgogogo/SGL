#include "mesh.h"
#include "component.h"

#include <cfloat>
#include <vector>

// ====================================================================================================
// NAMESPACE
// ====================================================================================================

namespace SGL
{
    // ====================================================================================================
    // STATIC
    // ====================================================================================================

    static std::vector<Model>    Meshes;
    static std::vector<int>      FreeNext;
    static int                   FreeList = -1;

    static std::vector<MeshPart> MeshParts;
    static std::vector<int>      MeshPartCounts;

    // ====================================================================================================
    // UTILS
    // ====================================================================================================

    static void ComputeMeshLocalBounds(const Mesh& mesh, Vector3& outMin, Vector3& outMax)
    {
        outMin = { FLT_MAX,  FLT_MAX,  FLT_MAX };
        outMax = { -FLT_MAX, -FLT_MAX, -FLT_MAX };

        for (int i = 0; i < mesh.vertexCount; i++)
        {
            float x = mesh.vertices[i * 3 + 0];
            float y = mesh.vertices[i * 3 + 1];
            float z = mesh.vertices[i * 3 + 2];

            outMin.x = fminf(outMin.x, x);
            outMin.y = fminf(outMin.y, y);
            outMin.z = fminf(outMin.z, z);

            outMax.x = fmaxf(outMax.x, x);
            outMax.y = fmaxf(outMax.y, y);
            outMax.z = fmaxf(outMax.z, z);
        }
    }

    static void ComputeModelParts(const Model& model, std::vector<MeshPart>& outParts)
    {
        outParts.clear();

        if (model.meshCount == 0) return;

        if (model.meshCount >= 1)
        {
            for (int i = 0; i < model.meshCount && (int)outParts.size() < MAX_MESH_PARTS; i++)
            {
                MeshPart part;
                ComputeMeshLocalBounds(model.meshes[i], part.localMin, part.localMax);
                outParts.push_back(part);
            }

            if ((int)outParts.size() >= model.meshCount) return;
        }

        Vector3 globalMin = { FLT_MAX,  FLT_MAX,  FLT_MAX };
        Vector3 globalMax = { -FLT_MAX, -FLT_MAX, -FLT_MAX };

        for (int i = 0; i < model.meshCount; i++)
        {
            Vector3 mn, mx;
            ComputeMeshLocalBounds(model.meshes[i], mn, mx);

            globalMin.x = fminf(globalMin.x, mn.x);
            globalMin.y = fminf(globalMin.y, mn.y);
            globalMin.z = fminf(globalMin.z, mn.z);

            globalMax.x = fmaxf(globalMax.x, mx.x);
            globalMax.y = fmaxf(globalMax.y, mx.y);
            globalMax.z = fmaxf(globalMax.z, mx.z);
        }

        int splitAxis = 0;
        float extX = globalMax.x - globalMin.x;
        float extY = globalMax.y - globalMin.y;
        float extZ = globalMax.z - globalMin.z;

        if (extY > extX && extY > extZ) splitAxis = 1;
        else if (extZ > extX && extZ > extY) splitAxis = 2;

        outParts.clear();

        for (int i = 0; i < MAX_MESH_PARTS; i++)
        {
            float t0 = (float)i / (float)MAX_MESH_PARTS;
            float t1 = (float)(i + 1) / (float)MAX_MESH_PARTS;

            MeshPart part;
            part.localMin = globalMin;
            part.localMax = globalMax;

            if (splitAxis == 0)
            {
                part.localMin.x = globalMin.x + extX * t0;
                part.localMax.x = globalMin.x + extX * t1;
            }
            else if (splitAxis == 1)
            {
                part.localMin.y = globalMin.y + extY * t0;
                part.localMax.y = globalMin.y + extY * t1;
            }
            else
            {
                part.localMin.z = globalMin.z + extZ * t0;
                part.localMax.z = globalMin.z + extZ * t1;
            }

            outParts.push_back(part);
        }
    }

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
            MeshPartCounts.push_back(0);
        }

        Meshes[handle] = model;
        FreeNext[handle] = -1;

        std::vector<MeshPart> parts;
        ComputeModelParts(model, parts);

        int oldCount = MeshPartCounts[handle];

        if ((int)MeshParts.size() < (handle + 1) * MAX_MESH_PARTS)
            MeshParts.resize((handle + 1) * MAX_MESH_PARTS);

        for (int i = 0; i < MAX_MESH_PARTS; i++)
        {
            if (i < (int)parts.size())
                MeshParts[handle * MAX_MESH_PARTS + i] = parts[i];
            else
                MeshParts[handle * MAX_MESH_PARTS + i] = MeshPart();
        }

        MeshPartCounts[handle] = (int)parts.size();

        return handle;
    }

    int RegisterMesh(const Mesh& mesh)
    {
        Model model = LoadModelFromMesh(mesh);
        return RegisterModel(model);
    }

    int RegisterMeshFromFile(const char* fileName)
    {
        Model model = LoadModel(fileName);

        if (model.meshCount == 0) return -1;

        return RegisterModel(model);
    }

    void UnregisterModel(int handle)
    {
        if (handle < 0) return;
        if (handle >= (int)Meshes.size()) return;
        if (Meshes[handle].meshCount == 0) return;

        UnloadModel(Meshes[handle]);
        Meshes[handle] = Model();
        MeshPartCounts[handle] = 0;

        FreeNext[handle] = FreeList;
        FreeList = handle;
    }

    int GetMeshPartCount(int handle)
    {
        if (handle < 0) return 0;
        if (handle >= (int)MeshPartCounts.size()) return 0;
        return MeshPartCounts[handle];
    }

    const MeshPart& GetMeshPart(int handle, int index)
    {
        static MeshPart dummy;
        if (handle < 0) return dummy;
        if (handle >= (int)MeshPartCounts.size()) return dummy;
        if (index < 0 || index >= MeshPartCounts[handle]) return dummy;
        return MeshParts[handle * MAX_MESH_PARTS + index];
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
        MeshPartCounts.clear();
        MeshParts.clear();
        FreeList = -1;
    }

}