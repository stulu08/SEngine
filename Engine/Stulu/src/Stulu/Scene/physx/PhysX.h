#pragma once
#include "Stulu/Core/Core.h"
#include "Stulu/Math/Math.h"


namespace physx {
    class PxFoundation;
    class PxPvd;
    class PxPhysics;
    class PxScene;
    class PxShape;
    class PxRigidDynamic;
    class PxTolerancesScale;
    class PxDefaultCpuDispatcher;
    class PxCudaContextManager;
    class PxTransform;
    class PxVec3;
    class PxQuat;
    class PxRigidActor;
    class PxRigidStatic;
    class PxTriangleMesh;
    class PxConvexMesh;
    class PxCooking;
    class PxControllerManager;
    class PxController;
}

namespace Stulu{
    class STULU_API TransformComponent;
    class STULU_API RigidbodyComponent;
    class STULU_API CharacterController;
    class STULU_API Mesh;


    STULU_API physx::PxVec3 Vec3ToPhysX(const glm::vec3& vec);
    STULU_API glm::vec3 PhysXToVec3(const physx::PxVec3& vec);

    STULU_API physx::PxQuat QuatToPhysX(const glm::quat& quat);
    STULU_API glm::quat PhysXToQuat(const physx::PxQuat& quat);

    struct PhysicsData {
        float speed = 9.81f;
        float length = 1.0f;
        glm::vec3 gravity = { 0.f, -9.8f, 0.f };
        uint32_t workerThreads = 4;
    };

    class STULU_API PhysX {
    public:
        PhysX(const PhysicsData& data = PhysicsData());
        ~PhysX();

        //call this on application startup
        static void startUp();
        //call this on application shutdown
        static void shutDown();

        static void startPVD();
        static void stopPVD();

        physx::PxController* createCapsuleController(CharacterController& controller, const TransformComponent& transform);
        physx::PxTriangleMesh* createTriangleMesh(Ref<Mesh>& mesh);
        physx::PxConvexMesh* createConvexMesh(Ref<Mesh>& mesh);

        physx::PxScene* getScene() const { return m_scene; };
        physx::PxPhysics* getPhysics() const { return m_physics; };
        physx::PxControllerManager* getControllerManager() const { return m_controllerManager; };

        static inline physx::PxFoundation* getFoundation() { return s_foundataion; };
        static inline bool isPVDRunning() { return s_pvdRunning; }
        static inline bool started() { return s_foundataion != nullptr; }
    private:
        static physx::PxFoundation* s_foundataion;
        static physx::PxPvd* s_pvd;
        static bool s_pvdRunning;
        static physx::PxCudaContextManager* s_cudaContextManager;

        physx::PxPhysics* m_physics = nullptr;
        physx::PxScene* m_scene = nullptr;
        physx::PxDefaultCpuDispatcher* m_cpuDispatcher = nullptr;
        physx::PxCooking* m_cooking = nullptr;
        physx::PxControllerManager* m_controllerManager = nullptr;
    };
}