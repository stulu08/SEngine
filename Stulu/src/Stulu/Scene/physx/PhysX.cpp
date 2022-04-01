#include "st_pch.h"
#include "PhysX.h"
#define PX_PHYSX_STATIC_LIB
#include "physx/PxPhysicsAPI.h"
#include "Stulu/Core/Time.h"
#include "Stulu/Scene/Components.h"

//https://gameworksdocs.nvidia.com/PhysX/4.0/documentation/PhysXGuide/Manual/API.html
namespace Stulu {
    class SnippetGpuLoadHook : public PxGpuLoadHook
    {
        virtual const char* getPhysXGpuDllName() const
        {
            return "data/PhysXGpu_64.dll";
        }
    } gGpuLoadHook;


    physx::PxVec3 PhysicsVec3fromglmVec3(const glm::vec3& vec) {
        return physx::PxVec3{ vec.x,vec.y,vec.z };
    }
    glm::vec3 PhysicsVec3toglmVec3(const physx::PxVec3& vec) {
        return glm::vec3{ vec.x,vec.y,vec.z };
    }
   
    physx::PxQuat PhysicsQuatfromglmQuat(const glm::quat& quat) {
        return physx::PxQuat(quat.x,quat.y,quat.z,quat.w);
    }
    glm::vec3 PhysicsQuattoglmVec3(const physx::PxQuat& quat) {
        glm::vec3 rotation;
        // From glm::decompose in matrix_decompose.inl

        using namespace glm;
        using T = float;

        mat4 LocalMatrix(toMat4(glm::quat(quat.w,quat.x,quat.y,quat.z)));

        // Normalize the matrix.
        //if (epsilonEqual(LocalMatrix[3][3], static_cast<float>(0), epsilon<T>()))
        //    return false;

        // First, isolate perspective.  This is the messiest.
        if (
            epsilonNotEqual(LocalMatrix[0][3], static_cast<T>(0), epsilon<T>()) ||
            epsilonNotEqual(LocalMatrix[1][3], static_cast<T>(0), epsilon<T>()) ||
            epsilonNotEqual(LocalMatrix[2][3], static_cast<T>(0), epsilon<T>()))
        {
            // Clear the perspective partition
            LocalMatrix[0][3] = LocalMatrix[1][3] = LocalMatrix[2][3] = static_cast<T>(0);
            LocalMatrix[3][3] = static_cast<T>(1);
        }

        // Next take care of translation (easy).
        //translation = vec3(LocalMatrix[3]);
        //LocalMatrix[3] = vec4(0, 0, 0, LocalMatrix[3].w);

        vec3 Row[3];

        // Now get scale and shear.
        for (length_t i = 0; i < 3; ++i)
            for (length_t j = 0; j < 3; ++j)
                Row[i][j] = LocalMatrix[i][j];

        // Compute X scale factor and normalize first row.
        //scale.x = length(Row[0]);
        Row[0] = detail::scale(Row[0], static_cast<T>(1));
        //scale.y = length(Row[1]);
        Row[1] = detail::scale(Row[1], static_cast<T>(1));
        //scale.z = length(Row[2]);
        Row[2] = detail::scale(Row[2], static_cast<T>(1));

        rotation.y = asin(-Row[0][2]);
        if (cos(rotation.y) != 0) {
            rotation.x = atan2(Row[1][2], Row[2][2]);
            rotation.z = atan2(Row[0][1], Row[0][0]);
        }
        else {
            rotation.x = atan2(-Row[2][0], Row[1][1]);
            rotation.z = 0;
        }
        return rotation;
    }
    
    physx::PxTransform PhysicsTransformfromTransformComponent(const glm::mat4& transform) {
        glm::vec3 position, rotation, scale;
        Math::decomposeTransformEuler(transform, position, rotation, scale);
        return physx::PxTransform(PhysicsVec3fromglmVec3(position), PhysicsQuatfromglmQuat(rotation));
    }
    
    class UserErrorCallback : public physx::PxErrorCallback {
    public:
        virtual void reportError(physx::PxErrorCode::Enum code, const char* message, const char* file, int line) {
            switch (code)
            {
            case physx::PxErrorCode::eNO_ERROR:
                break;
            case physx::PxErrorCode::eDEBUG_INFO:
                CORE_INFO("Phys debug info({1}:{2}): {0}", message, file, line);
                break;
            case physx::PxErrorCode::eDEBUG_WARNING:
                CORE_WARN("Phys debug warning({1}:{2}): {0}", message, file, line);
                break;
            case physx::PxErrorCode::eINVALID_PARAMETER:
                CORE_ERROR("Phys invalid parameter error({1}:{2}): {0}", message, file, line);
                break;
            case physx::PxErrorCode::eINVALID_OPERATION:
                CORE_ERROR("Phys invalid operation error({1}:{2}): {0}", message, file, line);
                break;
            case physx::PxErrorCode::eOUT_OF_MEMORY:
                CORE_ERROR("Phys out of memory error({1}:{2}): {0}", message, file, line);
                break;
            case physx::PxErrorCode::eINTERNAL_ERROR:
                CORE_ERROR("Phys internal error({1}:{2}): {0}", message, file, line);
                break;
            case physx::PxErrorCode::eABORT:
                CORE_CRITICAL("Phys Abort({1}:{2}): {0}", message, file, line);
                CORE_ASSERT(false, "PhysX error");
                break;
            case physx::PxErrorCode::ePERF_WARNING:
                CORE_WARN("Phys performance warning({0}:{1}): {2}", message, file, line);
                break;
            default:
                break;
            }
        }
    };
    void PhysX::startUp(const PhysicsData& data) {
        static UserErrorCallback gDefaultErrorCallback;
        static physx::PxDefaultAllocator gDefaultAllocatorCallback;

        physx::PxTolerancesScale tollerantScale = physx::PxTolerancesScale();
        tollerantScale.length = data.length;
        tollerantScale.speed = data.speed;

        m_foundataion = PxCreateFoundation(PX_PHYSICS_VERSION, gDefaultAllocatorCallback,
            gDefaultErrorCallback);
        if (!m_foundataion) {
            CORE_ERROR("PxCreateFoundation failed!");
            return;
        }

        bool recordMemoryAllocations = true;

        m_pvd = physx::PxCreatePvd(*m_foundataion);
        physx::PxPvdTransport* transport = physx::PxDefaultPvdSocketTransportCreate("localhost", 5425, 10);
        m_pvd->connect(*transport, physx::PxPvdInstrumentationFlag::eALL);

        m_physics = PxCreatePhysics(PX_PHYSICS_VERSION, *m_foundataion,
            tollerantScale, recordMemoryAllocations, m_pvd);
        if (!m_physics) {
            CORE_ERROR("PxCreatePhysics failed!");
            return;
        }
        if (!PxInitExtensions(*m_physics, m_pvd)) {
            CORE_ERROR("PxInitExtensions failed!");
            return;
        }
        m_cooking = PxCreateCooking(PX_PHYSICS_VERSION, *m_foundataion, physx::PxCookingParams(m_physics->getTolerancesScale()));
        if (!m_cooking) {
            CORE_ERROR("PxCreateCooking failed!");
            return;
        }
        PxSetPhysXGpuLoadHook(&gGpuLoadHook);

        m_cpuDispatcher = physx::PxDefaultCpuDispatcherCreate(data.workerThreads);
        if (!m_cpuDispatcher) {
            CORE_ERROR("PxDefaultCpuDispatcherCreate failed!");
            return;
        }

        physx::PxSceneDesc sceneDesc(m_physics->getTolerancesScale());

#if PX_WINDOWS
        // create GPU dispatcher
        physx::PxCudaContextManagerDesc cudaContextManagerDesc;
        m_cudaContextManager = PxCreateCudaContextManager(*m_foundataion, cudaContextManagerDesc);
        sceneDesc.cudaContextManager = m_cudaContextManager;
#endif

        sceneDesc.filterShader = physx::PxDefaultSimulationFilterShader;
        sceneDesc.cpuDispatcher = m_cpuDispatcher;
        sceneDesc.gravity = PhysicsVec3fromglmVec3(data.gravity);
        sceneDesc.flags |= physx::PxSceneFlag::eENABLE_PCM;
        sceneDesc.flags |= physx::PxSceneFlag::eENABLE_STABILIZATION;

        m_scene = m_physics->createScene(sceneDesc);
        if (!m_scene) {
            CORE_ERROR("Physics Scene creation failed!");
            return;
        }
    }
    void PhysX::shutDown() {
        if(m_scene)
            m_scene->release();
        if(m_cpuDispatcher)
            m_cpuDispatcher->release();
        if (m_cooking)
            m_cooking->release();
        PxCloseExtensions();
        if(m_physics)
            m_physics->release(); 
        if(m_pvd)
            m_pvd->release();
        if(m_foundataion)
            m_foundataion->release();

        m_scene = nullptr;
        m_cpuDispatcher = nullptr;
        m_cooking = nullptr;
        m_physics = nullptr;
        m_pvd = nullptr;
        m_foundataion = nullptr;
    }
    physx::PxRigidActor* PhysX::createActor(RigidbodyComponent& rb, const glm::vec3& pos, const glm::quat& rot) {
        physx::PxRigidActor* actor;
        if (rb.rbType == RigidbodyComponent::Type::Dynamic) {
            actor = m_physics->createRigidDynamic(physx::PxTransform(PhysicsVec3fromglmVec3(pos), PhysicsQuatfromglmQuat(rot)));
            actor->setActorFlag(physx::PxActorFlag::Enum::eDISABLE_GRAVITY, !rb.useGravity);
            physx::PxRigidDynamic* mactor = actor->is<physx::PxRigidDynamic>();
            mactor->setRigidBodyFlag(physx::PxRigidBodyFlag::Enum::eKINEMATIC, rb.kinematic);
            if (!rb.kinematic) {
                mactor->setRigidBodyFlag(physx::PxRigidBodyFlag::Enum::eRETAIN_ACCELERATIONS, rb.retainAccelaration);
            }
            physx::PxRigidBodyExt::updateMassAndInertia(*mactor, rb.mass, &PhysicsVec3fromglmVec3(rb.massCenterPos));
        }
        else {
            actor = m_physics->createRigidStatic(physx::PxTransform(PhysicsVec3fromglmVec3(pos), PhysicsQuatfromglmQuat(rot)));
            actor->setActorFlag(physx::PxActorFlag::Enum::eDISABLE_GRAVITY, !rb.useGravity);
        }
        rb.body = (void*)actor;
        return actor;
    }
    //mesh needs to be combined
    physx::PxTriangleMesh* PhysX::createTriangleMesh(Ref<Mesh>& mesh) {
        Ref<Mesh>& theMesh = createRef<Mesh>(Mesh::combine(*mesh.get()));

        physx::PxTriangleMeshDesc meshDesc;
        meshDesc.points.count = (uint32_t)theMesh->m_vertices.size();
        meshDesc.points.stride = sizeof(glm::vec3);
        meshDesc.points.data = &theMesh->m_vertices[0].pos;
        meshDesc.points.stride = sizeof(Vertex);
        meshDesc.points.data = theMesh->m_vertices.data();

        meshDesc.triangles.count = (uint32_t)theMesh->m_indices.size()/3;
        meshDesc.triangles.stride = 3 * sizeof(uint32_t);
        meshDesc.triangles.data = theMesh->m_indices.data();
        

        physx::PxDefaultMemoryOutputStream writeBuffer;
        physx::PxTriangleMeshCookingResult::Enum result;
        bool status = m_cooking->cookTriangleMesh(meshDesc, writeBuffer, &result);
        if (!status)
            return NULL;
        if (result == physx::PxTriangleMeshCookingResult::Enum::eFAILURE) {
            CORE_ERROR("Triangle Cooking failed");
        }
        physx::PxDefaultMemoryInputData readBuffer(writeBuffer.getData(), writeBuffer.getSize());
        return m_physics->createTriangleMesh(readBuffer);
    }
}