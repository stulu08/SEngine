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
    class PxConvexMesh;
    class PxCooking;
    class PxControllerManager;
    class PxController;
}
#define ST_DEFAULT_PHYSX_MATERIAL_STATIC_FRICTION .6f
#define ST_DEFAULT_PHYSX_MATERIAL_DYNAMIC_FRICTION .6f
#define ST_DEFAULT_PHYSX_MATERIAL_RESTITUTION .0f
namespace Stulu{
    class STULU_API TransformComponent;
    class STULU_API RigidbodyComponent;
    class STULU_API CharacterController;
    class STULU_API Mesh;
    class STULU_API PhysX;
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
    class STULU_API PhysX {
    public:
        PhysX() { }
        ~PhysX() { releasePhysics(); }
        //call this on application startup
        static void startUp();
        //call this on application shutdown
        static void shutDown();

        static void startPVD();
        static void stopPVD();

        void createPhysics(const PhysicsData& data = PhysicsData());
        void releasePhysics();

        physx::PxRigidActor* createActor(RigidbodyComponent& rb, const glm::vec3& pos, const glm::quat& rot);
        physx::PxRigidActor* createActor(float mass, bool kinematic, bool gravity, const glm::vec3& pos, const glm::quat& rot, const glm::vec3& massLocalCenter = glm::vec3(.0f));
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
        static inline physx::PxFoundation* s_foundataion = nullptr;
        static inline physx::PxPvd* s_pvd = nullptr;
        static inline bool s_pvdRunning = false;
        static inline physx::PxCudaContextManager* s_cudaContextManager = nullptr;

        physx::PxPhysics* m_physics = nullptr;
        physx::PxScene* m_scene = nullptr;
        physx::PxDefaultCpuDispatcher* m_cpuDispatcher = nullptr;
        physx::PxCooking* m_cooking = nullptr;
        physx::PxControllerManager* m_controllerManager = nullptr;
    };
}