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

    static Vector3 AmbientColor = { 1.0f, 1.0f, 1.0f };
    static float   AmbientIntensity = 0.1f;
    static int     AmbientColorLoc = -1;

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

        StandardShader = LoadShader("shaders/standard.vs", "shaders/standard.fs");
        StandardShaderLoaded = (StandardShader.id != 0);

        AmbientColorLoc = GetShaderLocation(StandardShader, "ambientColor");

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

    void GraphicsSetAmbientColor(float r, float g, float b, float intensity)
    {
        AmbientColor = { r, g, b };
        AmbientIntensity = intensity;
    }

    void GraphicsGetAmbientColor(float* r, float* g, float* b, float* intensity)
    {
        *r = AmbientColor.x;
        *g = AmbientColor.y;
        *b = AmbientColor.z;
        *intensity = AmbientIntensity;
    }

    void GraphicsRender()
    {
        InternalCamera& camera = GetCameraInstance();
        camera.UpdateCamera();

        Scene& scene = GetSceneInstance();
        scene.CollectRenderList();

        Vector3 ambient = {
            AmbientColor.x * AmbientIntensity,
            AmbientColor.y * AmbientIntensity,
            AmbientColor.z * AmbientIntensity
        };

        BeginDrawing();

        ClearBackground({ 0, 0, 0, 255 });

        BeginMode3D(camera.raylibCamera);

        SetShaderValue(StandardShader, AmbientColorLoc, &ambient, SHADER_UNIFORM_VEC3);

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