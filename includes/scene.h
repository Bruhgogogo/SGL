#pragma once

#include "raylibUtils.h"
#include "mathUtils.h"
#include "component.h"

#include <vector>

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

namespace SGL
{

    constexpr int INVALID_HANDLE = -1;

    template <typename T>
    class ComponentPool
    {
    public:
        void Add(int entity, const T& value);
        void Remove(int entity);
        bool Has(int entity) const;
        T& Get(int entity);
        const T& Get(int entity) const;

    private:
        std::vector<T>   data;
        std::vector<int> entities;
        std::vector<int> sparse;

        int Find(int entity) const;
    };

    class Scene
    {
    public:
        int  CreateEntity();
        void DestroyEntity(int handle);
        bool IsAlive(int handle) const;

        void SetParent(int child, int parent);
        int  GetParent(int handle) const;

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

        void UpdateWorld(int handle);

        void SetEntityPosition(int handle, float x, float y, float z);
        void GetEntityPosition(int handle, float* x, float* y, float* z);
        void GetEntityLocalPosition(int handle, float* x, float* y, float* z);

        void SetEntityRotation(int handle, float pitch, float yaw, float roll);
        void GetEntityRotation(int handle, float* pitch, float* yaw, float* roll);
        void GetEntityLocalRotation(int handle, float* pitch, float* yaw, float* roll);

        void SetEntityScale(int handle, float sx, float sy, float sz);
        void GetEntityScale(int handle, float* sx, float* sy, float* sz);
        void GetEntityLocalScale(int handle, float* sx, float* sy, float* sz);

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

        template <typename T>
        ComponentPool<T>& GetPool();

        bool IsValid(int handle) const;
        void AttachChild(int parent, int child);
        void DetachChild(int child);

        void UpdateWorldRecursive(int handle, const Transform& parentWorld, bool hasParentWorld);
    };

    SGL_DECLARE_COMPONENT(Transform);
    SGL_DECLARE_COMPONENT(Color);
    SGL_DECLARE_COMPONENT(Visible);

    Scene& GetSceneInstance();

    int  CreateEntity();
    void DestroyEntity(int handle);

    void SetParent(int child, int parent);
    int  GetParent(int handle);

    template <typename T>
    void AddComponent(int handle, const T& value);

    template <typename T>
    void RemoveComponent(int handle);

    template <typename T>
    bool HasComponent(int handle);

    template <typename T>
    T& GetComponent(int handle);

    void UpdateWorld(int handle);

    void SetEntityPosition(int handle, float x, float y, float z);
    void GetEntityPosition(int handle, float* x, float* y, float* z);
    void GetEntityLocalPosition(int handle, float* x, float* y, float* z);

    void SetEntityRotation(int handle, float pitch, float yaw, float roll);
    void GetEntityRotation(int handle, float* pitch, float* yaw, float* roll);
    void GetEntityLocalRotation(int handle, float* pitch, float* yaw, float* roll);

    void SetEntityScale(int handle, float sx, float sy, float sz);
    void GetEntityScale(int handle, float* sx, float* sy, float* sz);
    void GetEntityLocalScale(int handle, float* sx, float* sy, float* sz);

}