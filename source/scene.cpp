#include "scene.h"

// ====================================================================================================
// NAMESPACE
// ====================================================================================================

namespace SGL
{

    // ====================================================================================================
    // STATIC
    // ====================================================================================================

    static Scene SceneInstance;

    static OBB GetMeshBounds(int meshID)
    {
        OBB box;

        if (meshID == 0)
        {
            box.center = { 0, 0, 0 };
            box.halfExtents = { 0.5f, 0.5f, 0.5f };
            box.rotation = QuaternionIdentity();
        }
        else
        {
            box.center = { 0, 0, 0 };
            box.halfExtents = { 0, 0, 0 };
            box.rotation = QuaternionIdentity();
        }

        return box;
    }

    static bool OverlapOnAxis(
        const OBB& a, const OBB& b,
        const Vector3& axis,
        const Vector3& aAxisX, const Vector3& aAxisY, const Vector3& aAxisZ,
        const Vector3& bAxisX, const Vector3& bAxisY, const Vector3& bAxisZ)
    {
        float ra =
            fabsf(Vector3DotProduct(axis, aAxisX)) * a.halfExtents.x +
            fabsf(Vector3DotProduct(axis, aAxisY)) * a.halfExtents.y +
            fabsf(Vector3DotProduct(axis, aAxisZ)) * a.halfExtents.z;

        float rb =
            fabsf(Vector3DotProduct(axis, bAxisX)) * b.halfExtents.x +
            fabsf(Vector3DotProduct(axis, bAxisY)) * b.halfExtents.y +
            fabsf(Vector3DotProduct(axis, bAxisZ)) * b.halfExtents.z;

        Vector3 d = Vector3Subtract(b.center, a.center);
        float dist = fabsf(Vector3DotProduct(d, axis));

        return dist <= ra + rb;
    }

    static bool OBBIntersect(const OBB& a, const OBB& b)
    {
        Vector3 aX = Vector3RotateByQuaternion({ 1, 0, 0 }, a.rotation);
        Vector3 aY = Vector3RotateByQuaternion({ 0, 1, 0 }, a.rotation);
        Vector3 aZ = Vector3RotateByQuaternion({ 0, 0, 1 }, a.rotation);

        Vector3 bX = Vector3RotateByQuaternion({ 1, 0, 0 }, b.rotation);
        Vector3 bY = Vector3RotateByQuaternion({ 0, 1, 0 }, b.rotation);
        Vector3 bZ = Vector3RotateByQuaternion({ 0, 0, 1 }, b.rotation);

        if (!OverlapOnAxis(a, b, aX, aX, aY, aZ, bX, bY, bZ)) return false;
        if (!OverlapOnAxis(a, b, aY, aX, aY, aZ, bX, bY, bZ)) return false;
        if (!OverlapOnAxis(a, b, aZ, aX, aY, aZ, bX, bY, bZ)) return false;

        if (!OverlapOnAxis(a, b, bX, aX, aY, aZ, bX, bY, bZ)) return false;
        if (!OverlapOnAxis(a, b, bY, aX, aY, aZ, bX, bY, bZ)) return false;
        if (!OverlapOnAxis(a, b, bZ, aX, aY, aZ, bX, bY, bZ)) return false;

        Vector3 cross;

        cross = Vector3CrossProduct(aX, bX);
        if (Vector3Length(cross) > 1e-6f && !OverlapOnAxis(a, b, cross, aX, aY, aZ, bX, bY, bZ)) return false;

        cross = Vector3CrossProduct(aX, bY);
        if (Vector3Length(cross) > 1e-6f && !OverlapOnAxis(a, b, cross, aX, aY, aZ, bX, bY, bZ)) return false;

        cross = Vector3CrossProduct(aX, bZ);
        if (Vector3Length(cross) > 1e-6f && !OverlapOnAxis(a, b, cross, aX, aY, aZ, bX, bY, bZ)) return false;

        cross = Vector3CrossProduct(aY, bX);
        if (Vector3Length(cross) > 1e-6f && !OverlapOnAxis(a, b, cross, aX, aY, aZ, bX, bY, bZ)) return false;

        cross = Vector3CrossProduct(aY, bY);
        if (Vector3Length(cross) > 1e-6f && !OverlapOnAxis(a, b, cross, aX, aY, aZ, bX, bY, bZ)) return false;

        cross = Vector3CrossProduct(aY, bZ);
        if (Vector3Length(cross) > 1e-6f && !OverlapOnAxis(a, b, cross, aX, aY, aZ, bX, bY, bZ)) return false;

        cross = Vector3CrossProduct(aZ, bX);
        if (Vector3Length(cross) > 1e-6f && !OverlapOnAxis(a, b, cross, aX, aY, aZ, bX, bY, bZ)) return false;

        cross = Vector3CrossProduct(aZ, bY);
        if (Vector3Length(cross) > 1e-6f && !OverlapOnAxis(a, b, cross, aX, aY, aZ, bX, bY, bZ)) return false;

        cross = Vector3CrossProduct(aZ, bZ);
        if (Vector3Length(cross) > 1e-6f && !OverlapOnAxis(a, b, cross, aX, aY, aZ, bX, bY, bZ)) return false;

        return true;
    }

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

            if (HasComponent<OBB>(handle))
            {
                int meshID = HasComponent<MeshID>(handle) ? GetComponent<MeshID>(handle).value : -1;
                GetComponent<OBB>(handle) = ComputeOBB(worlds[handle], meshID);
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

        for (int i = 0; i < (int)alive.size(); i++)
        {
            if (!alive[i]) continue;
            if (!HasComponent<Transform>(i)) continue;
            if (!HasComponent<Color>(i)) continue;
            if (!HasComponent<MeshID>(i)) continue;
            if (!HasComponent<Visible>(i)) continue;
            if (GetComponent<Visible>(i).value == 0) continue;

            RenderItem item;
            item.transform = worlds[i];
            item.color = GetComponent<Color>(i);
            item.meshID = GetComponent<MeshID>(i).value;

            renderList.push_back(item);
        }
    }

    const std::vector<RenderItem>& Scene::GetRenderList() const
    {
        return renderList;
    }

    OBB Scene::ComputeOBB(const Transform& world, int meshID)
    {
        OBB local = GetMeshBounds(meshID);
        OBB result;

        Vector3 scaledCenter = {
            local.center.x * world.scale.x,
            local.center.y * world.scale.y,
            local.center.z * world.scale.z
        };

        Vector3 rotatedCenter = Vector3RotateByQuaternion(scaledCenter, world.rotation);
        result.center = Vector3Add(world.position, rotatedCenter);

        result.halfExtents = {
            local.halfExtents.x * world.scale.x,
            local.halfExtents.y * world.scale.y,
            local.halfExtents.z * world.scale.z
        };

        result.rotation = QuaternionMultiply(world.rotation, local.rotation);

        return result;
    }

    void Scene::SetEntityMeshID(int handle, int meshID)
    {
        if (!HasComponent<MeshID>(handle)) return;

        GetComponent<MeshID>(handle).value = meshID;

        if (HasComponent<OBB>(handle))
        {
            GetComponent<OBB>(handle) = ComputeOBB(worlds[handle], meshID);
        }
    }

    int Scene::CheckCollision(int a, int b)
    {
        if (!IsValid(a) || !IsValid(b)) return 0;

        if (!HasComponent<OBB>(a)) return 0;
        if (!HasComponent<OBB>(b)) return 0;

        if (!HasComponent<Physical>(a)) return 0;
        if (!HasComponent<Physical>(b)) return 0;

        if (GetComponent<Physical>(a).canCollide == 0) return 0;
        if (GetComponent<Physical>(b).canCollide == 0) return 0;

        OBB boxA = GetComponent<OBB>(a);
        OBB boxB = GetComponent<OBB>(b);

        return OBBIntersect(boxA, boxB) ? 1 : 0;
    }

    // ====================================================================================================
    // MACRO
    // ====================================================================================================

    SGL_INSTANTIATE_COMPONENT(Transform);
    SGL_INSTANTIATE_COMPONENT(Color);
    SGL_INSTANTIATE_COMPONENT(Visible);
    SGL_INSTANTIATE_COMPONENT(Physical);
    SGL_INSTANTIATE_COMPONENT(OBB);
    SGL_INSTANTIATE_COMPONENT(MeshID);
}