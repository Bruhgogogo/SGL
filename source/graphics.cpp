#include "graphics.h"
#include "scene.h"
#include "camera.h"

namespace SGL
{

    static Model CubeModel = {};
    static bool CubeModelLoaded = false;

    static void EnsureCubeModel()
    {
        if (CubeModelLoaded) return;

        Mesh mesh = GenMeshCube(1.0f, 1.0f, 1.0f);
        CubeModel = LoadModelFromMesh(mesh);
        CubeModelLoaded = true;
    }

    static Matrix MakeTransformMatrix(const Transform& t)
    {
        Matrix matScale = MatrixScale(t.scale.x, t.scale.y, t.scale.z);
        Matrix matRot = QuaternionToMatrix(t.rotation);
        Matrix matTrans = MatrixTranslate(t.position.x, t.position.y, t.position.z);

        Matrix mat = MatrixMultiply(matScale, matRot);
        mat = MatrixMultiply(mat, matTrans);

        return mat;
    }

    void GraphicsInit(int width, int height, const char* title)
    {
        InitWindow(width, height, title);
    }

    bool GraphicsShouldClose()
    {
        return WindowShouldClose();
    }

    void GraphicsRender()
    {
        EnsureCubeModel();

        InternalCamera& camera = GetCameraInstance();
        camera.UpdateCamera();

        Scene& scene = GetSceneInstance();
        scene.CollectRenderList();

        BeginDrawing();

        ClearBackground({ 0, 0, 0, 255 });

        BeginMode3D(camera.raylibCamera);

        const std::vector<RenderItem>& list = scene.GetRenderList();

        for (int i = 0; i < (int)list.size(); i++)
        {
            const RenderItem& item = list[i];

            if (item.meshID == 0)
            {
                CubeModel.transform = MakeTransformMatrix(item.transform);

                ::Color color = {
                    item.color.r,
                    item.color.g,
                    item.color.b,
                    item.color.a
                };

                DrawModel(CubeModel, { 0, 0, 0 }, 1.0f, color);
            }
        }

        EndMode3D();

        EndDrawing();
    }

}