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

    enum class PhsicsDebugViuals
    {
        /* RigidBody-related parameters */

        /** Visualization scale multiplier. Default: 0. */
        eGlobalSwitch,

        /** Visualize world axes. */
        eWorldAxes,

        /* Body visualizations */

        /** Visualize body axes. */
        eBodyAxes,
        /** Visualize body mass axes, indicating sleep states. */
        eBodyMassAxes,
        /** Visualize body linear velocity. */
        eBodyLinearVelocity,
        /** Visualize body angular velocity. */
        eBodyAngularVelocity,

        /* Contact visualizations */

        /** Visualize contact points, enabling contact info. */
        eContactPoint,
        /** Visualize contact normals, enabling contact info. */
        eContactNormal,
        /** Visualize contact errors, enabling contact info. */
        eContactError,
        /** Visualize contact forces, enabling contact info. */
        eContactForce,

        /* Actor visualizations */

        /** Visualize actor axes. */
        eActorAxes,
        /** Visualize collision bounds (AABBs in world space). */
        eCollisionAABBs,
        /** Visualize collision shapes. */
        eCollisionShapes,
        /** Visualize shape axes. */
        eCollisionAxes,
        /** Visualize compound AABBs in world space. */
        eCollisionCompounds,
        /** Visualize mesh and convex face normals. */
        eCollisionFaceNormals,
        /** Visualize active edges for meshes. */
        eCollisionEdges,
        /** Visualize static pruning structures. */
        eCollisionStatic,
        /** Visualize dynamic pruning structures. */
        eCollisionDynamic,
        /** Visualize pairwise state (deprecated). */
        eDeprecatedCollisionPairs,
        /** Visualize joint local axes. */
        eJointLocalFrames,
        /** Visualize joint limits. */
        eJointLimits,
        /** Visualize culling box. */
        eCullingBox,
        /** Visualize MBP regions. */
        eMBPRegions,
    };


    STULU_API physx::PxVec3 Vec3ToPhysX(const glm::vec3& vec);
    STULU_API glm::vec3 PhysXToVec3(const physx::PxVec3& vec);

    STULU_API physx::PxQuat QuatToPhysX(const glm::quat& quat);
    STULU_API glm::quat PhysXToQuat(const physx::PxQuat& quat);

    struct PhysicsData {
        float speed = 9.81f;
        float length = 1.0f;
        glm::vec3 gravity = { 0.f, -9.81f, 0.f };
        uint32_t workerThreads = 4;
    };

    class STULU_API PhysX {
    public:
        PhysX(const PhysicsData& data = PhysicsData(), float startTime = 0.0f);
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

        bool Advance(float timePoint);
        bool FetchResults();

        void SetDebugVisual(PhsicsDebugViuals option, float value) const;
        float GetDebugVisual(PhsicsDebugViuals option) const;
        // Call inside gizom renderer
        void RenderSceneDebugData() const;

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

        float m_time = 0.0f;
        physx::PxPhysics* m_physics = nullptr;
        physx::PxScene* m_scene = nullptr;
        physx::PxDefaultCpuDispatcher* m_cpuDispatcher = nullptr;
        physx::PxCooking* m_cooking = nullptr;
        physx::PxControllerManager* m_controllerManager = nullptr;
    };
}