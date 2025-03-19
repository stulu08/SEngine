#pragma once
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
    class PxMaterial;
    typedef uint32_t PxU32;
}

namespace Stulu {
    class STULU_API Mesh;

    namespace PhysicsHelper {
        enum class DebugViuals
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

        STULU_API physx::PxTriangleMesh* CreateTriangleMesh(Ref<Mesh>& mesh);
        STULU_API physx::PxConvexMesh* CreateConvexMesh(Ref<Mesh>& mesh);

        inline glm::vec4 ColorConvert(physx::PxU32 in) {
            const float s = 1.0f / 255.0f;
            return glm::vec4(
                ((in >> 0) & 0xFF) * s,
                ((in >> 8) & 0xFF) * s,
                ((in >> 16) & 0xFF) * s,
                ((in >> 24) & 0xFF) * s);
        }
    }
}