#include "scene.h"
#include "physical.h"

// ====================================================================================================
// NAMESPACE
// ====================================================================================================

namespace SGL
{

    // ====================================================================================================
    // STATIC
    // ====================================================================================================

    static Scene SceneInstance;

    // ====================================================================================================
    // APIs
    // ====================================================================================================

    Scene& GetSceneInstance()
    {
        return SceneInstance;
    }

    static Transform IdentityTransform()
    {
        Transform t;
        return t;
    }

    static Transform ComposeWorld(const Transform& parentWorld, const Transform& local)
    {
        Transform world;

        Vector3 scaled = {
            parentWorld.scale.x * local.position.x,
            parentWorld.scale.y * local.position.y,
            parentWorld.scale.z * local.position.z
        };

        Vector3 rotated = Vector3RotateByQuaternion(scaled, parentWorld.rotation);

        world.position = Vector3Add(parentWorld.position, rotated);
        world.rotation = QuaternionMultiply(parentWorld.rotation, local.rotation);
        world.scale = {
            parentWorld.scale.x * local.scale.x,
            parentWorld.scale.y * local.scale.y,
            parentWorld.scale.z * local.scale.z
        };

        return world;
    }

    static Transform InverseComposeWorld(const Transform& parentWorld, const Transform& world)
    {
        Transform local;

        Quaternion invRot = QuaternionInvert(parentWorld.rotation);

        Vector3 delta = Vector3Subtract(world.position, parentWorld.position);
        Vector3 rotated = Vector3RotateByQuaternion(delta, invRot);

        local.position = {
            rotated.x / parentWorld.scale.x,
            rotated.y / parentWorld.scale.y,
            rotated.z / parentWorld.scale.z
        };

        local.rotation = QuaternionMultiply(invRot, world.rotation);

        local.scale = {
            world.scale.x / parentWorld.scale.x,
            world.scale.y / parentWorld.scale.y,
            world.scale.z / parentWorld.scale.z
        };

        return local;
    }

    template <typename T>
    int ComponentPool<T>::Find(int entity) const
    {
        if (entity == INVALID_HANDLE) return -1;
        if (entity < 0) return -1;
        if (entity >= (int)sparse.size()) return -1;
        return sparse[entity];
    }

    template <typename T>
    void ComponentPool<T>::Add(int entity, const T& value)
    {
        int index = Find(entity);
        if (index != -1)
        {
            data[index] = value;
            return;
        }

        if (entity >= (int)sparse.size())
            sparse.resize(entity + 1, -1);

        sparse[entity] = (int)data.size();
        data.push_back(value);
        entities.push_back(entity);
    }

    template <typename T>
    void ComponentPool<T>::Remove(int entity)
    {
        int index = Find(entity);
        if (index == -1) return;

        int last = (int)data.size() - 1;

        data[index] = data[last];
        entities[index] = entities[last];

        sparse[entities[index]] = index;
        sparse[entity] = -1;

        data.pop_back();
        entities.pop_back();
    }

    template <typename T>
    bool ComponentPool<T>::Has(int entity) const
    {
        return Find(entity) != -1;
    }

    template <typename T>
    T& ComponentPool<T>::Get(int entity)
    {
        return data[sparse[entity]];
    }

    template <typename T>
    const T& ComponentPool<T>::Get(int entity) const
    {
        return data[sparse[entity]];
    }

    template <typename T>
    int& PoolIndex()
    {
        static int index = -1;
        return index;
    }

    template <typename T>
    static void PoolRemoveThunk(void* pool, int entity)
    {
        ((ComponentPool<T>*)pool)->Remove(entity);
    }

    template <typename T>
    ComponentPool<T>& Scene::GetPool()
    {
        int& index = PoolIndex<T>();

        if (index == -1)
        {
            index = (int)pools.size();
            pools.push_back(nullptr);
            poolRemovers.push_back(&PoolRemoveThunk<T>);
        }

        if (pools[index] == nullptr)
            pools[index] = new ComponentPool<T>();

        return *(ComponentPool<T>*)pools[index];
    }

    template <typename T>
    int ComponentPool<T>::Size() const
    {
        return (int)data.size();
    }

    template <typename T>
    int ComponentPool<T>::EntityAt(int index) const
    {
        return entities[index];
    }

    template <typename T>
    T& ComponentPool<T>::At(int index)
    {
        return data[index];
    }

    template <typename T>
    const T& ComponentPool<T>::At(int index) const
    {
        return data[index];
    }

    template <typename T>
    void Scene::AddComponent(int handle, const T& value)
    {
        if (!IsValid(handle)) return;
        GetPool<T>().Add(handle, value);
    }

    template <typename T>
    void Scene::RemoveComponent(int handle)
    {
        if (!IsValid(handle)) return;
        GetPool<T>().Remove(handle);
    }

    template <typename T>
    bool Scene::HasComponent(int handle) const
    {
        if (!IsValid(handle)) return false;
        return ((Scene*)this)->GetPool<T>().Has(handle);
    }

    template <typename T>
    T& Scene::GetComponent(int handle)
    {
        return GetPool<T>().Get(handle);
    }

    template <typename T>
    const T& Scene::GetComponent(int handle) const
    {
        return ((Scene*)this)->GetPool<T>().Get(handle);
    }

    bool Scene::IsValid(int handle) const
    {
        if (handle == INVALID_HANDLE) return false;
        if (handle < 0) return false;
        if (handle >= (int)alive.size()) return false;
        return alive[handle] != 0;
    }

    bool Scene::IsAlive(int handle) const
    {
        return IsValid(handle);
    }

    void Scene::AttachChild(int parent, int child)
    {
        parents[child] = parent;
        nextSiblings[child] = firstChilds[parent];
        prevSiblings[child] = INVALID_HANDLE;

        if (nextSiblings[child] != INVALID_HANDLE)
            prevSiblings[nextSiblings[child]] = child;

        firstChilds[parent] = child;
    }

    void Scene::DetachChild(int child)
    {
        if (prevSiblings[child] != INVALID_HANDLE)
            nextSiblings[prevSiblings[child]] = nextSiblings[child];
        else if (parents[child] != INVALID_HANDLE)
            firstChilds[parents[child]] = nextSiblings[child];

        if (nextSiblings[child] != INVALID_HANDLE)
            prevSiblings[nextSiblings[child]] = prevSiblings[child];

        parents[child] = INVALID_HANDLE;
        nextSiblings[child] = INVALID_HANDLE;
        prevSiblings[child] = INVALID_HANDLE;
    }

    int Scene::CreateEntity()
    {
        int handle;

        if (freeList != INVALID_HANDLE)
        {
            handle = freeList;
            freeList = freeNext[handle];
        }
        else
        {
            handle = (int)alive.size();

            parents.push_back(INVALID_HANDLE);
            firstChilds.push_back(INVALID_HANDLE);
            nextSiblings.push_back(INVALID_HANDLE);
            prevSiblings.push_back(INVALID_HANDLE);
            alive.push_back(0);
            freeNext.push_back(INVALID_HANDLE);
            worlds.push_back(Transform());
        }

        parents[handle] = INVALID_HANDLE;
        firstChilds[handle] = INVALID_HANDLE;
        nextSiblings[handle] = INVALID_HANDLE;
        prevSiblings[handle] = INVALID_HANDLE;
        freeNext[handle] = INVALID_HANDLE;
        worlds[handle] = Transform();
        alive[handle] = 1;

        return handle;
    }

    void Scene::DestroyEntity(int handle)
    {
        if (!IsValid(handle)) return;

        int c = firstChilds[handle];

        while (c != INVALID_HANDLE)
        {
            int next = nextSiblings[c];
            DestroyEntity(c);
            c = next;
        }

        DetachChild(handle);

        for (int i = 0; i < (int)pools.size(); i++)
        {
            if (pools[i] != nullptr)
                poolRemovers[i](pools[i], handle);
        }

        firstChilds[handle] = INVALID_HANDLE;
        nextSiblings[handle] = INVALID_HANDLE;
        prevSiblings[handle] = INVALID_HANDLE;
        worlds[handle] = Transform();
        alive[handle] = 0;

        freeNext[handle] = freeList;
        freeList = handle;
    }

    void Scene::SetParent(int child, int parent)
    {
        if (!IsValid(child)) return;
        if (child == parent) return;

        bool hasLocal = HasComponent<Transform>(child);

        Transform world = IdentityTransform();
        if (hasLocal)
            world = worlds[child];

        DetachChild(child);

        if (parent != INVALID_HANDLE && IsValid(parent))
            AttachChild(parent, child);

        if (hasLocal)
        {
            Transform parentWorld = IdentityTransform();
            bool hasParentWorld = false;

            if (parent != INVALID_HANDLE && HasComponent<Transform>(parent))
            {
                parentWorld = worlds[parent];
                hasParentWorld = true;
            }

            if (hasParentWorld)
                GetComponent<Transform>(child) = InverseComposeWorld(parentWorld, world);
            else
                GetComponent<Transform>(child) = world;

            UpdateEntityTree(child);
        }
    }

    int Scene::GetParent(int handle) const
    {
        if (!IsValid(handle)) return INVALID_HANDLE;
        return parents[handle];
    }

    int Scene::GetFirstChild(int handle) const
    {
        if (!IsValid(handle)) return INVALID_HANDLE;
        return firstChilds[handle];
    }

    int Scene::GetNextSibling(int handle) const
    {
        if (!IsValid(handle)) return INVALID_HANDLE;
        return nextSiblings[handle];
    }

    void Scene::UpdateWorldRecursive(int handle, const Transform& parentWorld, bool hasParentWorld)
    {
        bool hasLocal = HasComponent<Transform>(handle);

        if (hasLocal)
        {
            Transform& local = GetComponent<Transform>(handle);

            if (hasParentWorld)
                worlds[handle] = ComposeWorld(parentWorld, local);
            else
                worlds[handle] = local;

            if (HasComponent<CapsuleShape>(handle) || HasComponent<CylinderShape>(handle))
            {
                float maxRadial = fmaxf(worlds[handle].scale.x, worlds[handle].scale.z);
                worlds[handle].scale.x = maxRadial;
                worlds[handle].scale.z = maxRadial;
            }

            worlds[handle].matrixDirty = 1;

            ShapeUpdateWorld(handle, worlds[handle]);

            if (HasComponent<AABB>(handle))
            {
                GetComponent<AABB>(handle).dirty = 1;
            }
        }

        Transform currentWorld = hasLocal ? worlds[handle] : parentWorld;
        bool currentValid = hasLocal ? true : hasParentWorld;

        int c = firstChilds[handle];

        while (c != INVALID_HANDLE)
        {
            int next = nextSiblings[c];
            UpdateWorldRecursive(c, currentWorld, currentValid);
            c = next;
        }
    }

    void Scene::UpdateEntityTree(int handle)
    {
        if (!IsValid(handle)) return;

        Transform parentWorld = IdentityTransform();
        bool hasParentWorld = false;

        if (parents[handle] != INVALID_HANDLE)
        {
            int p = parents[handle];

            if (HasComponent<Transform>(p))
            {
                parentWorld = worlds[p];
                hasParentWorld = true;
            }
        }

        UpdateWorldRecursive(handle, parentWorld, hasParentWorld);
    }

    void Scene::SetEntityPosition(int handle, float x, float y, float z)
    {
        if (!HasComponent<Transform>(handle)) return;

        GetComponent<Transform>(handle).position = { x, y, z };
        UpdateEntityTree(handle);

        if (!HasComponent<Physical>(handle)) return;

        if (GetComponent<Physical>(handle).anchored) return;

        GetJoltWorldInstance().TeleportBody(handle, x, y, z);
    }

    void Scene::GetEntityPosition(int handle, float* x, float* y, float* z)
    {
        if (!IsValid(handle)) return;

        Vector3 position = worlds[handle].position;

        *x = position.x;
        *y = position.y;
        *z = position.z;
    }

    void Scene::GetEntityLocalPosition(int handle, float* x, float* y, float* z)
    {
        if (!HasComponent<Transform>(handle)) return;

        Vector3 position = GetComponent<Transform>(handle).position;

        *x = position.x;
        *y = position.y;
        *z = position.z;
    }

    void Scene::SetEntityRotation(int handle, float pitch, float yaw, float roll)
    {
        if (!HasComponent<Transform>(handle)) return;

        float qx, qy, qz, qw;
        EulerToQuat(pitch, yaw, roll, qx, qy, qz, qw);

        GetComponent<Transform>(handle).rotation = { qx, qy, qz, qw };
        UpdateEntityTree(handle);
    }

    void Scene::GetEntityRotation(int handle, float* pitch, float* yaw, float* roll)
    {
        if (!IsValid(handle)) return;

        Quaternion q = worlds[handle].rotation;

        QuatToEuler(q.x, q.y, q.z, q.w, *pitch, *yaw, *roll);
    }

    void Scene::GetEntityLocalRotation(int handle, float* pitch, float* yaw, float* roll)
    {
        if (!HasComponent<Transform>(handle)) return;

        Quaternion q = GetComponent<Transform>(handle).rotation;

        QuatToEuler(q.x, q.y, q.z, q.w, *pitch, *yaw, *roll);
    }

    void Scene::SetEntityScale(int handle, float sx, float sy, float sz)
    {
        if (!HasComponent<Transform>(handle)) return;

        GetComponent<Transform>(handle).scale = { sx, sy, sz };

        if (HasComponent<BoxShape>(handle))      GetComponent<BoxShape>(handle).dirty = 1;
        if (HasComponent<SphereShape>(handle))   GetComponent<SphereShape>(handle).dirty = 1;
        if (HasComponent<CapsuleShape>(handle))  GetComponent<CapsuleShape>(handle).dirty = 1;
        if (HasComponent<CylinderShape>(handle)) GetComponent<CylinderShape>(handle).dirty = 1;

        UpdateEntityTree(handle);
    }

    void Scene::GetEntityScale(int handle, float* sx, float* sy, float* sz)
    {
        if (!IsValid(handle)) return;

        Vector3 scale = worlds[handle].scale;

        *sx = scale.x;
        *sy = scale.y;
        *sz = scale.z;
    }

    void Scene::GetEntityLocalScale(int handle, float* sx, float* sy, float* sz)
    {
        if (!HasComponent<Transform>(handle)) return;

        Vector3 scale = GetComponent<Transform>(handle).scale;

        *sx = scale.x;
        *sy = scale.y;
        *sz = scale.z;
    }

    void Scene::CollectRenderList()
    {
        renderList.clear();

        ComponentPool<MeshID>& meshPool = GetPool<MeshID>();

        for (int i = 0; i < meshPool.Size(); i++)
        {
            int entity = meshPool.EntityAt(i);

            if (!IsAlive(entity)) continue;
            if (!HasComponent<Transform>(entity)) continue;
            if (!HasComponent<Color>(entity)) continue;
            if (!HasComponent<Visible>(entity)) continue;
            if (GetComponent<Visible>(entity).value == 0) continue;

            const Color& color = GetComponent<Color>(entity);
            if (color.a == 0) continue;

            RenderItem item;
            item.transform = worlds[entity];
            item.color = color;
            item.meshID = meshPool.At(i).value;

            renderList.push_back(item);
        }
    }

    const std::vector<RenderItem>& Scene::GetRenderList() const
    {
        return renderList;
    }

    void Scene::CollectLightList()
    {
        directionalLightList.clear();
        pointLightList.clear();
        spotLightList.clear();

        for (int handle = 0; handle < (int)alive.size(); handle++)
        {
            if (!IsAlive(handle)) continue;
            if (!HasComponent<Transform>(handle)) continue;
            if (!HasComponent<Color>(handle)) continue;
            if (!HasComponent<Visible>(handle)) continue;
            if (GetComponent<Visible>(handle).value == 0) continue;

            const Transform& world = GetWorldTransform(handle);
            const Color& color = GetComponent<Color>(handle);

            Vector3 rgb = {
                color.r / 255.0f,
                color.g / 255.0f,
                color.b / 255.0f
            };

            if (HasComponent<DirectionalLight>(handle))
            {
                const DirectionalLight& light = GetComponent<DirectionalLight>(handle);

                DirectionalLightItem item;
                item.direction = Vector3RotateByQuaternion({ 0.0f, -1.0f, 0.0f }, world.rotation);
                item.color = rgb;
                item.intensity = light.intensity;

                directionalLightList.push_back(item);
            }

            if (HasComponent<PointLight>(handle))
            {
                const PointLight& light = GetComponent<PointLight>(handle);

                if (light.range <= 0.0f) continue;

                PointLightItem item;
                item.position = world.position;
                item.color = rgb;
                item.intensity = light.intensity;
                item.range = light.range;

                pointLightList.push_back(item);
            }

            if (HasComponent<SpotLight>(handle))
            {
                const SpotLight& light = GetComponent<SpotLight>(handle);

                if (light.range <= 0.0f) continue;

                SpotLightItem item;
                item.position = world.position;
                item.direction = Vector3RotateByQuaternion({ 0.0f, 0.0f, -1.0f }, world.rotation);
                item.color = rgb;
                item.intensity = light.intensity;
                item.range = light.range;
                item.cosInner = cosf(light.innerAngle);
                item.cosOuter = cosf(light.outerAngle);

                spotLightList.push_back(item);
            }
        }
    }

    const std::vector<DirectionalLightItem>& Scene::GetDirectionalLightList() const
    {
        return directionalLightList;
    }

    const std::vector<PointLightItem>& Scene::GetPointLightList() const
    {
        return pointLightList;
    }

    const std::vector<SpotLightItem>& Scene::GetSpotLightList() const
    {
        return spotLightList;
    }

    void Scene::SetEntityMeshID(int handle, int meshID)
    {
        if (!HasComponent<MeshID>(handle)) return;

        GetComponent<MeshID>(handle).value = meshID;
    }

    int Scene::GetEntityCount() const
    {
        return (int)alive.size();
    }

    Transform& Scene::GetWorldTransform(int handle)
    {
        return worlds[handle];
    }

    const Transform& Scene::GetWorldTransform(int handle) const
    {
        return worlds[handle];
    }

    void Scene::SetWorldTransform(int handle, const Transform& world)
    {
        if (!IsValid(handle)) return;

        worlds[handle] = world;

        if (HasComponent<AABB>(handle))
        {
            GetComponent<AABB>(handle).dirty = 1;
        }

        if (HasComponent<Transform>(handle))
        {
            Transform parentWorld = IdentityTransform();
            bool hasParentWorld = false;

            if (parents[handle] != INVALID_HANDLE)
            {
                int p = parents[handle];

                if (HasComponent<Transform>(p))
                {
                    parentWorld = worlds[p];
                    hasParentWorld = true;
                }
            }

            if (hasParentWorld)
                GetComponent<Transform>(handle) = InverseComposeWorld(parentWorld, world);
            else
                GetComponent<Transform>(handle) = world;

            UpdateEntityTree(handle);
        }
    }

    // ====================================================================================================
    // MACRO
    // ====================================================================================================

    SGL_INSTANTIATE_COMPONENT(Transform);
    SGL_INSTANTIATE_COMPONENT(Color);
    SGL_INSTANTIATE_COMPONENT(Visible);
    SGL_INSTANTIATE_COMPONENT(Physical);
    SGL_INSTANTIATE_COMPONENT(BoxShape);
    SGL_INSTANTIATE_COMPONENT(SphereShape);
    SGL_INSTANTIATE_COMPONENT(CapsuleShape);
    SGL_INSTANTIATE_COMPONENT(CylinderShape);
    SGL_INSTANTIATE_COMPONENT(MeshShape);
    SGL_INSTANTIATE_COMPONENT(AABB);
    SGL_INSTANTIATE_COMPONENT(MeshID);
    SGL_INSTANTIATE_COMPONENT(DirectionalLight);
    SGL_INSTANTIATE_COMPONENT(PointLight);
    SGL_INSTANTIATE_COMPONENT(SpotLight);
}