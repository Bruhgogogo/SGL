#pragma once

#include "raylibUtils.h"

// ====================================================================================================
// NAMESPACE
// ====================================================================================================

namespace SGL
{
    // ====================================================================================================
    // APIs
    // ====================================================================================================

    int  RegisterMesh(const Mesh& mesh);
    int  RegisterModel(const Model& model);
    void UnregisterModel(int handle);

    Model& GetMeshModel(int handle);
    bool   IsMeshValid(int handle);

    void MeshShutdown();

}