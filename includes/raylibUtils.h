#pragma once

#include "Raylib/raylib.h"
#include "Raylib/raymath.h"
#include "Raylib/rlgl.h"

#ifdef _WIN32

#pragma comment(lib, "winmm.lib")

#ifdef _DEBUG
#pragma comment(lib, "Raylib_Debug.lib")
#else
#pragma comment(lib, "Raylib_Release.lib")
#endif

#endif

#include <vector>

// ====================================================================================================
// NAMESPACE
// ====================================================================================================

namespace SGL
{
    // ====================================================================================================
    // MESH GENERATION
    // ====================================================================================================

    inline Mesh GenMeshCapsule(float radius, float height, int slices, int rings)
    {
        if (slices < 3) slices = 3;
        if (rings < 2) rings = 2;

        std::vector<float> vertices;
        std::vector<float> normals;
        std::vector<float> texcoords;
        std::vector<unsigned short> indices;

        float halfHeight = height * 0.5f;

        float ringStep = (PI * 0.5f) / (float)rings;
        float sliceStep = 2.0f * PI / (float)slices;

        int sliceCount = slices + 1;
        int hemiRingCount = rings + 1;

        int cylinderBase = (int)(vertices.size() / 3);

        for (int r = 0; r <= 1; r++)
        {
            float y = (r == 0) ? -halfHeight : halfHeight;

            for (int s = 0; s <= slices; s++)
            {
                float angle = s * sliceStep;
                float x = cosf(angle) * radius;
                float z = sinf(angle) * radius;

                vertices.push_back(x);
                vertices.push_back(y);
                vertices.push_back(z);

                normals.push_back(cosf(angle));
                normals.push_back(0.0f);
                normals.push_back(sinf(angle));

                texcoords.push_back((float)s / (float)slices);
                texcoords.push_back((float)r);
            }
        }

        for (int s = 0; s < slices; s++)
        {
            int i0 = cylinderBase + s;
            int i1 = cylinderBase + s + 1;
            int i2 = cylinderBase + sliceCount + s;
            int i3 = cylinderBase + sliceCount + s + 1;

            indices.push_back(i0);
            indices.push_back(i2);
            indices.push_back(i1);

            indices.push_back(i1);
            indices.push_back(i2);
            indices.push_back(i3);
        }

        int topBase = (int)(vertices.size() / 3);

        for (int r = 0; r <= rings; r++)
        {
            float phi = r * ringStep;
            float y = sinf(phi) * radius;
            float rxz = cosf(phi) * radius;

            for (int s = 0; s <= slices; s++)
            {
                float angle = s * sliceStep;
                float x = cosf(angle) * rxz;
                float z = sinf(angle) * rxz;

                vertices.push_back(x);
                vertices.push_back(y + halfHeight);
                vertices.push_back(z);

                normals.push_back(cosf(angle) * cosf(phi));
                normals.push_back(sinf(phi));
                normals.push_back(sinf(angle) * cosf(phi));

                texcoords.push_back((float)s / (float)slices);
                texcoords.push_back((float)r / (float)rings);
            }
        }

        for (int r = 0; r < rings; r++)
        {
            for (int s = 0; s < slices; s++)
            {
                int i0 = topBase + r * sliceCount + s;
                int i1 = i0 + 1;
                int i2 = topBase + (r + 1) * sliceCount + s;
                int i3 = i2 + 1;

                indices.push_back(i0);
                indices.push_back(i2);
                indices.push_back(i1);

                indices.push_back(i1);
                indices.push_back(i2);
                indices.push_back(i3);
            }
        }

        int bottomBase = (int)(vertices.size() / 3);

        for (int r = 0; r <= rings; r++)
        {
            float phi = r * ringStep;
            float y = -sinf(phi) * radius;
            float rxz = cosf(phi) * radius;

            for (int s = 0; s <= slices; s++)
            {
                float angle = s * sliceStep;
                float x = cosf(angle) * rxz;
                float z = sinf(angle) * rxz;

                vertices.push_back(x);
                vertices.push_back(y - halfHeight);
                vertices.push_back(z);

                normals.push_back(cosf(angle) * cosf(phi));
                normals.push_back(-sinf(phi));
                normals.push_back(sinf(angle) * cosf(phi));

                texcoords.push_back((float)s / (float)slices);
                texcoords.push_back((float)r / (float)rings);
            }
        }

        for (int r = 0; r < rings; r++)
        {
            for (int s = 0; s < slices; s++)
            {
                int i0 = bottomBase + r * sliceCount + s;
                int i1 = i0 + 1;
                int i2 = bottomBase + (r + 1) * sliceCount + s;
                int i3 = i2 + 1;

                indices.push_back(i0);
                indices.push_back(i1);
                indices.push_back(i2);

                indices.push_back(i1);
                indices.push_back(i3);
                indices.push_back(i2);
            }
        }

        Mesh mesh = { 0 };

        int vertexCount = (int)(vertices.size() / 3);
        int indexCount = (int)indices.size();

        mesh.vertexCount = vertexCount;
        mesh.triangleCount = indexCount / 3;

        mesh.vertices = (float*)MemAlloc(vertices.size() * sizeof(float));
        mesh.normals = (float*)MemAlloc(normals.size() * sizeof(float));
        mesh.texcoords = (float*)MemAlloc(texcoords.size() * sizeof(float));
        mesh.indices = (unsigned short*)MemAlloc(indices.size() * sizeof(unsigned short));

        for (size_t i = 0; i < vertices.size(); i++)
            mesh.vertices[i] = vertices[i];

        for (size_t i = 0; i < normals.size(); i++)
            mesh.normals[i] = normals[i];

        for (size_t i = 0; i < texcoords.size(); i++)
            mesh.texcoords[i] = texcoords[i];

        for (size_t i = 0; i < indices.size(); i++)
            mesh.indices[i] = indices[i];

        UploadMesh(&mesh, false);

        return mesh;
    }
}