#pragma once

#include "raylibUtils.h"

// ====================================================================================================
// NAMESPACE
// ====================================================================================================

namespace SGL
{
    // ====================================================================================================
    // STRUCT
    // ====================================================================================================

    struct MeshPart
    {
        Vector3 localMin = { -0.5f, -0.5f, -0.5f };
        Vector3 localMax = { 0.5f,  0.5f,  0.5f };
    };

    // ====================================================================================================
    // APIs
    // ====================================================================================================

    int  RegisterMesh(const Mesh& mesh);
    int  RegisterModel(const Model& model);
    int  RegisterMeshFromFile(const char* fileName);
    void UnregisterModel(int handle);

    int  GetMeshPartCount(int handle);
    const MeshPart& GetMeshPart(int handle, int index);

    Model& GetMeshModel(int handle);
    bool   IsMeshValid(int handle);

    void MeshShutdown();

}