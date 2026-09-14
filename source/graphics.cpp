#include "graphics.h"
#include "scene.h"
#include "camera.h"
#include <iostream>

// ====================================================================================================
// NAMESPACE
// ====================================================================================================

namespace SGL
{
    // ====================================================================================================
    // STATIC
    // ====================================================================================================

    static Model CubeModel = {};
    static Model SphereModel = {};
    static Model CapsuleModel = {};
    static Model CylinderModel = {};

    static Shader StandardShader = {};
    static bool StandardShaderLoaded = false;

    static Vector3 GroundAmbient = { 0.2f, 0.15f, 0.1f };
    static Vector3 SkyAmbient = { 0.5f, 0.6f, 0.8f };
    static int     GroundAmbientLoc = -1;
    static int     SkyAmbientLoc = -1;

    static const int MaxDirLights = 2;

    static int DirLightCountLoc = -1;
    static int DirLightDirectionsLoc = -1;
    static int DirLightColorsLoc = -1;
    static int DirLightIntensitiesLoc = -1;

    static const int MaxPointLights = 8;

    static int PointLightCountLoc = -1;
    static int PointLightPositionsLoc = -1;
    static int PointLightColorsLoc = -1;
    static int PointLightIntensitiesLoc = -1;
    static int PointLightRangesLoc = -1;

    // ====================================================================================================
    // UTILS
    // ====================================================================================================

    static void EnsureCubeModel()
    {
        Mesh mesh = GenMeshCube(1.0f, 1.0f, 1.0f);
        CubeModel = LoadModelFromMesh(mesh);
        CubeModel.materials[0].shader = StandardShader;
    }

    static void EnsureSphereModel()
    {
        Mesh mesh = GenMeshSphere(0.5f, 16, 16);
        SphereModel = LoadModelFromMesh(mesh);
        SphereModel.materials[0].shader = StandardShader;
    }

    static void EnsureCapsuleModel()
    {
        Mesh mesh = GenMeshCapsule(0.5f, 1.0f, 16, 16);
        CapsuleModel = LoadModelFromMesh(mesh);
        CapsuleModel.materials[0].shader = StandardShader;
    }

    static void EnsureCylinderModel()
    {
        Mesh mesh = GenMeshCylinder(0.5f, 1.0f, 16);
        CylinderModel = LoadModelFromMesh(mesh);
        CylinderModel.materials[0].shader = StandardShader;
    }

    static Matrix MakeTransformMatrix(const Transform& t)
    {
        if (t.matrixDirty == 0)
            return t.cachedMatrix;

        Matrix matScale = MatrixScale(t.scale.x, t.scale.y, t.scale.z);
        Matrix matRot = QuaternionToMatrix(t.rotation);
        Matrix matTrans = MatrixTranslate(t.position.x, t.position.y, t.position.z);

        Matrix mat = MatrixMultiply(matScale, matRot);
        mat = MatrixMultiply(mat, matTrans);

        t.cachedMatrix = mat;
        t.matrixDirty = 0;

        return mat;
    }

    // ====================================================================================================
    // APIs
    // ====================================================================================================

    void GraphicsInit(int width, int height, const char* title)
    {
        InitWindow(width, height, title);

        StandardShader = LoadShader("shaders/standard.vert", "shaders/standard.frag");
        StandardShaderLoaded = (StandardShader.id != 0);

        GroundAmbientLoc = GetShaderLocation(StandardShader, "groundAmbient");
        SkyAmbientLoc = GetShaderLocation(StandardShader, "skyAmbient");

        DirLightCountLoc = GetShaderLocation(StandardShader, "dirLightCount");
        DirLightDirectionsLoc = GetShaderLocation(StandardShader, "dirLightDirections");
        DirLightColorsLoc = GetShaderLocation(StandardShader, "dirLightColors");
        DirLightIntensitiesLoc = GetShaderLocation(StandardShader, "dirLightIntensities");

        PointLightCountLoc = GetShaderLocation(StandardShader, "pointLightCount");
        PointLightPositionsLoc = GetShaderLocation(StandardShader, "pointLightPositions");
        PointLightColorsLoc = GetShaderLocation(StandardShader, "pointLightColors");
        PointLightIntensitiesLoc = GetShaderLocation(StandardShader, "pointLightIntensities");
        PointLightRangesLoc = GetShaderLocation(StandardShader, "pointLightRanges");

        EnsureCubeModel();
        EnsureSphereModel();
        EnsureCapsuleModel();
        EnsureCylinderModel();
    }

    void GraphicsShutdown()
    {
        if (StandardShaderLoaded)
        {
            UnloadShader(StandardShader);
            StandardShaderLoaded = false;
        }
    }

    bool GraphicsShouldClose()
    {
        return WindowShouldClose();
    }

    void GraphicsGetWindowWidth(int* width)
    {
        *width = GetScreenWidth();
    }

    void GraphicsGetWindowHeight(int* height)
    {
        *height = GetScreenHeight();
    }

    void GraphicsSetWindowMode(int mode)
    {
        ::ClearWindowState(FLAG_FULLSCREEN_MODE | FLAG_BORDERLESS_WINDOWED_MODE);

        if (mode == 1)
            ::SetWindowState(FLAG_BORDERLESS_WINDOWED_MODE);
        else if (mode == 2)
            ::SetWindowState(FLAG_FULLSCREEN_MODE);
    }

    void GraphicsSetWindowWidth(int width)
    {
        ::SetWindowSize(width, GetScreenHeight());
    }

    void GraphicsSetWindowHeight(int height)
    {
        ::SetWindowSize(GetScreenWidth(), height);
    }

    void GraphicsGetWindowMode(int* mode)
    {
        if (::IsWindowState(FLAG_FULLSCREEN_MODE))
            *mode = 2;
        else if (::IsWindowState(FLAG_BORDERLESS_WINDOWED_MODE))
            *mode = 1;
        else
            *mode = 0;
    }

    void GraphicsSetGroundAmbient(float r, float g, float b)
    {
        GroundAmbient = { r, g, b };
    }

    void GraphicsGetGroundAmbient(float* r, float* g, float* b)
    {
        *r = GroundAmbient.x;
        *g = GroundAmbient.y;
        *b = GroundAmbient.z;
    }

    void GraphicsSetSkyAmbient(float r, float g, float b)
    {
        SkyAmbient = { r, g, b };
    }

    void GraphicsGetSkyAmbient(float* r, float* g, float* b)
    {
        *r = SkyAmbient.x;
        *g = SkyAmbient.y;
        *b = SkyAmbient.z;
    }

    void AABBUpdateWorld(int handle)
    {
        Scene& scene = GetSceneInstance();

        if (!scene.HasComponent<AABB>(handle)) return;

        AABB& aabb = scene.GetComponent<AABB>(handle);

        if (aabb.dirty == 0) return;

        const Transform& world = scene.GetWorldTransform(handle);

        Vector3 localMin = { 0, 0, 0 };
        Vector3 localMax = { 0, 0, 0 };

        if (scene.HasComponent<BoxShape>(handle))
        {
            BoxShape& shape = scene.GetComponent<BoxShape>(handle);
            localMin = Vector3Negate(shape.worldHalfExtents);
            localMax = shape.worldHalfExtents;
        }
        else if (scene.HasComponent<SphereShape>(handle))
        {
            SphereShape& shape = scene.GetComponent<SphereShape>(handle);
            localMin = { -shape.worldRadius, -shape.worldRadius, -shape.worldRadius };
            localMax = { shape.worldRadius,  shape.worldRadius,  shape.worldRadius };
        }
        else if (scene.HasComponent<CapsuleShape>(handle))
        {
            CapsuleShape& shape = scene.GetComponent<CapsuleShape>(handle);
            float h = shape.worldHeight * 0.5f + shape.worldRadius;
            localMin = { -shape.worldRadius, -h, -shape.worldRadius };
            localMax = { shape.worldRadius,  h,  shape.worldRadius };
        }
        else if (scene.HasComponent<CylinderShape>(handle))
        {
            CylinderShape& shape = scene.GetComponent<CylinderShape>(handle);
            float h = shape.worldHeight * 0.5f;
            localMin = { -shape.worldRadius, -h, -shape.worldRadius };
            localMax = { shape.worldRadius,  h,  shape.worldRadius };
        }

        Vector3 corners[8] = {
            { localMin.x, localMin.y, localMin.z },
            { localMax.x, localMin.y, localMin.z },
            { localMin.x, localMax.y, localMin.z },
            { localMax.x, localMax.y, localMin.z },
            { localMin.x, localMin.y, localMax.z },
            { localMax.x, localMin.y, localMax.z },
            { localMin.x, localMax.y, localMax.z },
            { localMax.x, localMax.y, localMax.z }
        };

        Vector3 worldMin = { FLT_MAX,  FLT_MAX,  FLT_MAX };
        Vector3 worldMax = { -FLT_MAX, -FLT_MAX, -FLT_MAX };

        for (int i = 0; i < 8; i++)
        {
            Vector3 p = Vector3RotateByQuaternion(corners[i], world.rotation);
            p = Vector3Add(p, world.position);

            worldMin.x = fminf(worldMin.x, p.x);
            worldMin.y = fminf(worldMin.y, p.y);
            worldMin.z = fminf(worldMin.z, p.z);

            worldMax.x = fmaxf(worldMax.x, p.x);
            worldMax.y = fmaxf(worldMax.y, p.y);
            worldMax.z = fmaxf(worldMax.z, p.z);
        }

        aabb.worldMin = worldMin;
        aabb.worldMax = worldMax;

        aabb.dirty = 0;
    }

    void GraphicsRender()
    {
        InternalCamera& camera = GetCameraInstance();
        camera.UpdateCamera();

        Scene& scene = GetSceneInstance();
        scene.CollectRenderList();

        BeginDrawing();

        ClearBackground({ 0, 0, 0, 255 });

        BeginMode3D(camera.raylibCamera);

        SetShaderValue(StandardShader, GroundAmbientLoc, &GroundAmbient, SHADER_UNIFORM_VEC3);
        SetShaderValue(StandardShader, SkyAmbientLoc, &SkyAmbient, SHADER_UNIFORM_VEC3);

        scene.CollectLightList();

        const std::vector<DirectionalLightItem>& dirLightList = scene.GetDirectionalLightList();

        float dirLightDirections[MaxDirLights * 3] = {};
        float dirLightColors[MaxDirLights * 3] = {};
        float dirLightIntensities[MaxDirLights] = {};

        int dirLightCount = 0;

        for (int i = 0; i < (int)dirLightList.size() && dirLightCount < MaxDirLights; i++)
        {
            const DirectionalLightItem& light = dirLightList[i];

            dirLightDirections[dirLightCount * 3 + 0] = light.direction.x;
            dirLightDirections[dirLightCount * 3 + 1] = light.direction.y;
            dirLightDirections[dirLightCount * 3 + 2] = light.direction.z;

            dirLightColors[dirLightCount * 3 + 0] = light.color.x;
            dirLightColors[dirLightCount * 3 + 1] = light.color.y;
            dirLightColors[dirLightCount * 3 + 2] = light.color.z;

            dirLightIntensities[dirLightCount] = light.intensity;

            dirLightCount++;
        }

        SetShaderValue(StandardShader, DirLightCountLoc, &dirLightCount, SHADER_UNIFORM_INT);

        if (dirLightCount > 0)
        {
            SetShaderValueV(StandardShader, DirLightDirectionsLoc, dirLightDirections, SHADER_UNIFORM_VEC3, dirLightCount);
            SetShaderValueV(StandardShader, DirLightColorsLoc, dirLightColors, SHADER_UNIFORM_VEC3, dirLightCount);
            SetShaderValueV(StandardShader, DirLightIntensitiesLoc, dirLightIntensities, SHADER_UNIFORM_FLOAT, dirLightCount);
        }

        const std::vector<PointLightItem>& pointLightList = scene.GetPointLightList();

        float pointLightPositions[MaxPointLights * 3] = {};
        float pointLightColors[MaxPointLights * 3] = {};
        float pointLightIntensities[MaxPointLights] = {};
        float pointLightRanges[MaxPointLights] = {};

        int pointLightCount = 0;

        for (int i = 0; i < (int)pointLightList.size() && pointLightCount < MaxPointLights; i++)
        {
            const PointLightItem& light = pointLightList[i];

            pointLightPositions[pointLightCount * 3 + 0] = light.position.x;
            pointLightPositions[pointLightCount * 3 + 1] = light.position.y;
            pointLightPositions[pointLightCount * 3 + 2] = light.position.z;

            pointLightColors[pointLightCount * 3 + 0] = light.color.x;
            pointLightColors[pointLightCount * 3 + 1] = light.color.y;
            pointLightColors[pointLightCount * 3 + 2] = light.color.z;

            pointLightIntensities[pointLightCount] = light.intensity;
            pointLightRanges[pointLightCount] = light.range;

            pointLightCount++;
        }

        SetShaderValue(StandardShader, PointLightCountLoc, &pointLightCount, SHADER_UNIFORM_INT);

        if (pointLightCount > 0)
        {
            SetShaderValueV(StandardShader, PointLightPositionsLoc, pointLightPositions, SHADER_UNIFORM_VEC3, pointLightCount);
            SetShaderValueV(StandardShader, PointLightColorsLoc, pointLightColors, SHADER_UNIFORM_VEC3, pointLightCount);
            SetShaderValueV(StandardShader, PointLightIntensitiesLoc, pointLightIntensities, SHADER_UNIFORM_FLOAT, pointLightCount);
            SetShaderValueV(StandardShader, PointLightRangesLoc, pointLightRanges, SHADER_UNIFORM_FLOAT, pointLightCount);
        }

        const std::vector<RenderItem>& list = scene.GetRenderList();

        for (int i = 0; i < (int)list.size(); i++)
        {
            const RenderItem& item = list[i];

            ::Color color = {
                item.color.r,
                item.color.g,
                item.color.b,
                item.color.a
            };

            if (item.meshID == 0)
            {
                CubeModel.transform = MakeTransformMatrix(item.transform);
                DrawModel(CubeModel, { 0, 0, 0 }, 1.0f, color);
            }
            else if (item.meshID == 1)
            {
                SphereModel.transform = MakeTransformMatrix(item.transform);
                DrawModel(SphereModel, { 0, 0, 0 }, 1.0f, color);
            }
            else if (item.meshID == 2)
            {
                CapsuleModel.transform = MakeTransformMatrix(item.transform);
                DrawModel(CapsuleModel, { 0, 0, 0 }, 1.0f, color);
            }
            else if (item.meshID == 3)
            {
                CylinderModel.transform = MakeTransformMatrix(item.transform);
                DrawModel(CylinderModel, { 0, 0, 0 }, 1.0f, color);
            }
        }

        EndMode3D();

        EndDrawing();
    }

}