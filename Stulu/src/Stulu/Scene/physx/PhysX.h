#pragma once
#include "Stulu/Core/Core.h"
#include "Stulu/Math/Math.h"
namespace physx {
    class PxFoundation;
    class PxPvd;
    class PxPhysics;
    class PxScene;
    class PxTolerancesScale;
    class PxDefaultCpuDispatcher;
    class PxCudaContextManager;
    class PxTransform;
    class PxVec3;
    class PxQuat;
    class PxRigidActor;
    class PxTriangleMesh;
    class PxCooking;
}

namespace Stulu{
    struct TransformComponent;
    struct RigidbodyComponent;
    class Mesh;
    physx::PxVec3 PhysicsVec3fromglmVec3(const glm::vec3& vec);
    glm::vec3 PhysicsVec3toglmVec3(const physx::PxVec3& vec);

    physx::PxQuat PhysicsQuatfromglmQuat(const glm::quat& quat);
    glm::vec3 PhysicsQuattoglmVec3(const physx::PxQuat& quat);

    physx::PxTransform PhysicsTransformfromTransformComponent(const glm::mat4& transform);

    struct PhysicsData {
        float speed = 9.81f;
        float length = 1.0f;
        glm::vec3 gravity = { 0.f, -9.8f, 0.f };
        uint32_t workerThreads = 4;
    };
    class PxAllocatorCallback {
    public:
        virtual ~PxAllocatorCallback() {}
        virtual void* allocate(size_t size, const char* typeName, const char* filename,
            int line) = 0;
        virtual void deallocate(void* ptr) = 0;
    };
    class PhysX {
    public:
        void startUp(const PhysicsData& data = PhysicsData());
        void shutDown();

        physx::PxRigidActor* createActor(RigidbodyComponent& rb, const glm::vec3& pos, const glm::vec3& rot);
        physx::PxTriangleMesh* createTriangleMesh(Ref<Mesh>& mesh);

        physx::PxScene* getScene() { return m_scene; };
        physx::PxPhysics* getPhysics() { return m_physics; };

    private:
        physx::PxFoundation* m_foundataion;
        physx::PxPvd* m_pvd;
        physx::PxPhysics* m_physics;
        physx::PxScene* m_scene;
        physx::PxDefaultCpuDispatcher* m_cpuDispatcher;
        physx::PxCudaContextManager* m_cudaContextManager;
        physx::PxCooking* m_cooking;
    };
}