#pragma once

#include "raylibUtils.h"
#include "mathUtils.h"
#include "component.h"

#include <vector>

// ====================================================================================================
// MACRO
// ====================================================================================================

#define SGL_DECLARE_COMPONENT(TYPE)                                            \
    extern template class ComponentPool<TYPE>;                                 \
    extern template void Scene::AddComponent<TYPE>(int handle, const TYPE& value); \
    extern template void Scene::RemoveComponent<TYPE>(int handle);             \
    extern template bool Scene::HasComponent<TYPE>(int handle) const;          \
    extern template TYPE& Scene::GetComponent<TYPE>(int handle);               \
    extern template const TYPE& Scene::GetComponent<TYPE>(int handle) const;

#define SGL_INSTANTIATE_COMPONENT(TYPE)                                        \
    template class ComponentPool<TYPE>;                                        \
    template void Scene::AddComponent<TYPE>(int handle, const TYPE& value);    \
    template void Scene::RemoveComponent<TYPE>(int handle);                    \
    template bool Scene::HasComponent<TYPE>(int handle) const;                 \
    template TYPE& Scene::GetComponent<TYPE>(int handle);                      \
    template const TYPE& Scene::GetComponent<TYPE>(int handle) const;

// ====================================================================================================
// NAMESPACE
// ====================================================================================================

namespace SGL
{
    // ====================================================================================================
    // CONSTEXPR
    // ====================================================================================================

    constexpr int INVALID_HANDLE = -1;

    // ====================================================================================================
    // COMPONENT POOL
    // ====================================================================================================

    template <typename T>
    class ComponentPool
    {
    public:
        void Add(int entity, const T& value);
        void Remove(int entity);
        bool Has(int entity) const;
        T& Get(int entity);
        const T& Get(int entity) const;

        int Size() const;
        int EntityAt(int index) const;
        T& At(int index);
        const T& At(int index) const;

    private:
        std::vector<T>   data;
        std::vector<int> entities;
        std::vector<int> sparse;

        int Find(int entity) const;
    };

    // ====================================================================================================
    // SCENE
    // ====================================================================================================

    class Scene
    {
    public:
        int  CreateEntity();
        void DestroyEntity(int handle);
        bool IsAlive(int handle) const;

        void SetParent(int child, int parent);
        int  GetParent(int handle) const;

        int  GetFirstChild(int handle) const;
        int  GetNextSibling(int handle) const;

        template <typename T>
        void AddComponent(int handle, const T& value);

        template <typename T>
        void RemoveComponent(int handle);

        template <typename T>
        bool HasComponent(int handle) const;

        template <typename T>
        T& GetComponent(int handle);

        template <typename T>
        const T& GetComponent(int handle) const;

        void UpdateEntityTree(int handle);

        void CollectRenderList();
        const std::vector<RenderItem>& GetRenderList() const;

        void CollectLightList();
        const std::vector<DirectionalLightItem>& GetDirectionalLightList() const;

        void SetEntityPosition(int handle, float x, float y, float z);
        void GetEntityPosition(int handle, float* x, float* y, float* z);
        void GetEntityLocalPosition(int handle, float* x, float* y, float* z);

        void SetEntityRotation(int handle, float pitch, float yaw, float roll);
        void GetEntityRotation(int handle, float* pitch, float* yaw, float* roll);
        void GetEntityLocalRotation(int handle, float* pitch, float* yaw, float* roll);

        void SetEntityScale(int handle, float sx, float sy, float sz);
        void GetEntityScale(int handle, float* sx, float* sy, float* sz);
        void GetEntityLocalScale(int handle, float* sx, float* sy, float* sz);

        void SetEntityMeshID(int handle, int meshID);

        int GetEntityCount() const;

        Transform& GetWorldTransform(int handle);
        const Transform& GetWorldTransform(int handle) const;

        void SetWorldTransform(int handle, const Transform& world);

    private:
        std::vector<int> parents;
        std::vector<int> firstChilds;
        std::vector<int> nextSiblings;
        std::vector<int> prevSiblings;

        std::vector<int> alive;
        std::vector<int> freeNext;
        int freeList = INVALID_HANDLE;

        std::vector<Transform> worlds;

        std::vector<void*> pools;
        std::vector<void (*)(void*, int)> poolRemovers;

        std::vector<RenderItem> renderList;
        std::vector<DirectionalLightItem> directionalLightList;

        template <typename T>
        ComponentPool<T>& GetPool();

        bool IsValid(int handle) const;
        void AttachChild(int parent, int child);
        void DetachChild(int child);

        void UpdateWorldRecursive(int handle, const Transform& parentWorld, bool hasParentWorld);
    };

    // ====================================================================================================
    // MACRO
    // ====================================================================================================

    SGL_DECLARE_COMPONENT(Transform);
    SGL_DECLARE_COMPONENT(Color);
    SGL_DECLARE_COMPONENT(Visible);
    SGL_DECLARE_COMPONENT(Physical);
    SGL_DECLARE_COMPONENT(BoxShape);
    SGL_DECLARE_COMPONENT(SphereShape);
    SGL_DECLARE_COMPONENT(CapsuleShape);
    SGL_DECLARE_COMPONENT(CylinderShape);
    SGL_DECLARE_COMPONENT(AABB);
    SGL_DECLARE_COMPONENT(MeshID);
    SGL_DECLARE_COMPONENT(DirectionalLight);

    // ====================================================================================================
    // APIs
    // ====================================================================================================

    Scene& GetSceneInstance();

}