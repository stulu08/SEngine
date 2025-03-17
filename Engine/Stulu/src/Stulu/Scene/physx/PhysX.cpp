#include "st_pch.h"

#define PX_PHYSX_STATIC_LIB
#include "PhysX.h"
#include "PxPhysicsAPI.h"

#include "Stulu/Core/Time.h"
#include "Stulu/Scene/Components/Components.h"
#include "Stulu/Scene/physx/CharacterController.h"
#include "Stulu/ImGui/Gizmo.h"

namespace physx {
    class StDefaultAllocator : public physx::PxAllocatorCallback
    {
    public:
        void* allocate(size_t size, const char*, const char*, int)
        {
            void* ptr = platformAlignedAlloc(size);
            PX_ASSERT((reinterpret_cast<size_t>(ptr) & 15) == 0);
            return ptr;
        }

        void deallocate(void* ptr)
        {
            platformAlignedFree(ptr);
        }
    };
}

//https://gameworksdocs.nvidia.com/PhysX/4.0/documentation/PhysXGuide/Manual/API.html
namespace Stulu {
    physx::PxFoundation* PhysX::s_foundataion = nullptr;
    physx::PxPvd* PhysX::s_pvd = nullptr;
    bool PhysX::s_pvdRunning = nullptr;
    physx::PxCudaContextManager* PhysX::s_cudaContextManager = nullptr;

    class SnippetGpuLoadHook : public PxGpuLoadHook
    {
        virtual const char* getPhysXGpuDllName() const
        {
            return "Data/PhysX/PhysXGpu_64.dll";
        }
    } gGpuLoadHook;

    inline glm::vec4 ColorConvert(physx::PxU32 in) {
        const float s = 1.0f / 255.0f;
        return glm::vec4(
            ((in >> 0) & 0xFF) * s,
            ((in >> 8) & 0xFF) * s,
            ((in >> 16) & 0xFF) * s,
            ((in >> 24) & 0xFF) * s);
        
    }

    physx::PxVec3 Vec3ToPhysX(const glm::vec3& vec) {
        return physx::PxVec3{ vec.x,vec.y,vec.z };
    }
    glm::vec3 PhysXToVec3(const physx::PxVec3& vec) {
        return glm::vec3{ vec.x,vec.y,vec.z };
    }
   
    physx::PxQuat QuatToPhysX(const glm::quat& quat) {
        return physx::PxQuat(quat.x,quat.y,quat.z,quat.w);
    }
    glm::quat PhysXToQuat(const physx::PxQuat& quat) {
        return glm::quat(quat.w, quat.x, quat.y, quat.z);
    }
    
    class UserErrorCallback : public physx::PxErrorCallback {
    public:
        virtual void reportError(physx::PxErrorCode::Enum code, const char* message, const char* file, int line) {
            switch (code)
            {
            case physx::PxErrorCode::eNO_ERROR:
                break;
            case physx::PxErrorCode::eDEBUG_INFO:
                CORE_INFO("PhysX debug info({1}:{2}): {0}", message, file, line);
                break;
            case physx::PxErrorCode::eDEBUG_WARNING:
                CORE_WARN("PhysX debug warning({1}:{2}): {0}", message, file, line);
                break;
            case physx::PxErrorCode::eINVALID_PARAMETER:
                CORE_ERROR("PhysX invalid parameter error({1}:{2}): {0}", message, file, line);
                break;
            case physx::PxErrorCode::eINVALID_OPERATION:
                CORE_ERROR("PhysX invalid operation error({1}:{2}): {0}", message, file, line);
                break;
            case physx::PxErrorCode::eOUT_OF_MEMORY:
                CORE_ERROR("PhysX out of memory error({1}:{2}): {0}", message, file, line);
                break;
            case physx::PxErrorCode::eINTERNAL_ERROR:
                CORE_ERROR("PhysX internal error({1}:{2}): {0}", message, file, line);
                break;
            case physx::PxErrorCode::eABORT:
                CORE_CRITICAL("PhysX Abort({1}:{2}): {0}", message, file, line);
                CORE_ASSERT(false, "PhysX error");
                break;
            case physx::PxErrorCode::ePERF_WARNING:
                CORE_WARN("PhysX performance warning({0}:{1}): {2}", message, file, line);
                break;
            default:
                break;
            }
        }
    };

    void PhysX::startPVD() {
        if (isPVDRunning())
            stopPVD();
        s_pvd = physx::PxCreatePvd(*s_foundataion);
        physx::PxPvdTransport* transport = physx::PxDefaultPvdSocketTransportCreate("localhost", 5425, 10);
        s_pvd->connect(*transport, physx::PxPvdInstrumentationFlag::eALL);
        s_pvdRunning = true;
    }
    void PhysX::stopPVD() {
        if (s_pvd->isConnected())
            s_pvd->disconnect();
        if (s_pvd)
            s_pvd->release();
        s_pvdRunning = false;
    }

    PhysX::PhysX(const PhysicsData& data, float startTime) {
        m_time = startTime;

        bool recordMemoryAllocations = true;
        physx::PxTolerancesScale tollerantScale;
        tollerantScale.length = data.length;
        tollerantScale.speed = data.speed;

        m_physics = PxCreatePhysics(PX_PHYSICS_VERSION, *s_foundataion, tollerantScale, recordMemoryAllocations, s_pvd);
        if (!m_physics) {
            CORE_ERROR("PxCreatePhysics failed!");
            return;
        }

        // Only initialize extensions if needed
        if (!PxInitExtensions(*m_physics, s_pvd)) {
            CORE_ERROR("PxInitExtensions failed!");
            return;
        }

        PxRegisterHeightFields(*m_physics);

        m_cooking = PxCreateCooking(PX_PHYSICS_VERSION, m_physics->getFoundation(), physx::PxCookingParams(m_physics->getTolerancesScale()));
        if (!m_cooking) {
            CORE_ERROR("PxCreateCooking failed!");
            return;
        }

        m_cpuDispatcher = physx::PxDefaultCpuDispatcherCreate(glm::clamp(data.workerThreads, 1u, std::thread::hardware_concurrency()));
        if (!m_cpuDispatcher) {
            CORE_ERROR("PxDefaultCpuDispatcherCreate failed!");
            return;
        }

        physx::PxSceneDesc sceneDesc(m_physics->getTolerancesScale());

#if PX_WINDOWS
        sceneDesc.cudaContextManager = s_cudaContextManager;
#endif
        sceneDesc.filterShader = physx::PxDefaultSimulationFilterShader;
        sceneDesc.cpuDispatcher = m_cpuDispatcher;
        sceneDesc.gravity = Vec3ToPhysX(data.gravity);
        sceneDesc.flags |= physx::PxSceneFlag::eENABLE_PCM;
        sceneDesc.flags |= physx::PxSceneFlag::eENABLE_STABILIZATION;

        m_scene = m_physics->createScene(sceneDesc);
        if (!m_scene) {
            CORE_ERROR("Physics Scene creation failed!");
            return;
        }

        m_controllerManager = PxCreateControllerManager(*m_scene);
        if (!m_controllerManager) {
            CORE_ERROR("Physics Controller Manager creation failed!");
            return;
        }
        
        // Verify gravity was applied correctly
        physx::PxVec3 currentGravity = m_scene->getGravity();
        CORE_INFO("Physics scene created with gravity: ({}, {}, {})", currentGravity.x, currentGravity.y, currentGravity.z);
    }

    PhysX::~PhysX() {
        if (m_controllerManager) {
            m_controllerManager->release();
            m_controllerManager = nullptr;
        }
        if (m_scene) {
            m_scene->release();
            m_scene = nullptr;
        }
        if (m_cpuDispatcher) {
            m_cpuDispatcher->release();
            m_cpuDispatcher = nullptr;
        }
        if (m_cooking) {
            m_cooking->release();
            m_cooking = nullptr;
        }

        PxCloseExtensions();

        if (m_physics) {
            m_physics->release();
            m_physics = nullptr;
        }
    }

    void PhysX::startUp() {
        static UserErrorCallback gDefaultErrorCallback;
        static physx::StDefaultAllocator gDefaultAllocatorCallback;

        s_foundataion = PxCreateFoundation(PX_PHYSICS_VERSION, gDefaultAllocatorCallback, gDefaultErrorCallback);
        if (!s_foundataion) {
            CORE_ERROR("PxCreateFoundation failed!");
            return;
        }
        
        // Optional: Enable GPU load hook only if GPU support is required
        PxSetPhysXGpuLoadHook(&gGpuLoadHook);

#if PX_WINDOWS
        // Create GPU dispatcher
        physx::PxCudaContextManagerDesc cudaContextManagerDesc;
        s_cudaContextManager = PxCreateCudaContextManager(*s_foundataion, cudaContextManagerDesc);
        if (!s_cudaContextManager || !s_cudaContextManager->contextIsValid()) {
            CORE_WARN("PxCreateCudaContextManager failed or is invalid!");
            s_cudaContextManager = nullptr;
        }
#endif
    }
    void PhysX::shutDown() {
        if (s_cudaContextManager) {
            s_cudaContextManager->releaseContext();
            s_cudaContextManager->release();
            s_cudaContextManager = nullptr;
        }
        if (s_pvd) {
            s_pvd->release();
            s_pvd = nullptr;
        }
        if (s_foundataion) {
            s_foundataion->release();
            s_foundataion = nullptr;
        }
    }
    
    physx::PxController* PhysX::createCapsuleController(CharacterController& contr, const TransformComponent& transform) {
        physx::PxCapsuleControllerDesc desc;

        desc.position = physx::PxExtendedVec3(transform.GetWorldPosition().x, transform.GetWorldPosition().y, transform.GetWorldPosition().z);
        desc.upDirection = Vec3ToPhysX(transform.GetUp());

        desc.height = contr.height;
        desc.radius = contr.radius;
        desc.slopeLimit = glm::cos(glm::radians(contr.slopeOffset));
        desc.contactOffset = contr.skinWidth;//skin width
        desc.stepOffset = contr.stepOffset;
        desc.density = contr.density;

        desc.nonWalkableMode = (physx::PxControllerNonWalkableMode::Enum)((int)contr.nonWalkAbleMode);
        desc.climbingMode = (physx::PxCapsuleClimbingMode::Enum)((int)contr.climbingMode);
        
        contr.material.CreateMaterial(this);
        desc.material = contr.material.GetMaterial();
        
        desc.invisibleWallHeight = contr.m_invisibleWallHeight;
        desc.maxJumpHeight = contr.m_maxJumpHeight;
        desc.scaleCoeff = contr.m_scaleCoeff;

        physx::PxController* controller = m_controllerManager->createController(desc);
        contr.controller = (void*)controller;
        return controller;
    }
    //mesh needs to be combined
    physx::PxTriangleMesh* PhysX::createTriangleMesh(Ref<Mesh>& mesh) {
        Ref<Mesh>& theMesh = createRef<Mesh>(Mesh::combine(*mesh.get()));

        physx::PxTriangleMeshDesc meshDesc;
        meshDesc.points.count = theMesh->getVerticesCount();
        meshDesc.points.stride = theMesh->getVertexBuffer()->getStride();
        meshDesc.points.data = theMesh->m_vertices.data();

        meshDesc.triangles.count = theMesh->getIndicesCount() / 3;
        meshDesc.triangles.stride = 3 * theMesh->getIndexBuffer()->getStride();
        meshDesc.triangles.data = theMesh->m_indices.data();


        physx::PxDefaultMemoryOutputStream writeBuffer;
        physx::PxTriangleMeshCookingResult::Enum result;
        bool status = m_cooking->cookTriangleMesh(meshDesc, writeBuffer, &result);
        if (!status)
            return NULL;
        if (result == physx::PxTriangleMeshCookingResult::Enum::eFAILURE) {
            CORE_ERROR("Triangle Cooking failed");
            return nullptr;
        }
        physx::PxDefaultMemoryInputData readBuffer(writeBuffer.getData(), writeBuffer.getSize());
        return m_physics->createTriangleMesh(readBuffer);
    }
    physx::PxConvexMesh* PhysX::createConvexMesh(Ref<Mesh>& mesh) {
        Ref<Mesh>& theMesh = createRef<Mesh>(Mesh::combine(*mesh.get()));
        physx::PxConvexMeshDesc convexDesc;
        convexDesc.points.count = theMesh->getVerticesCount();
        convexDesc.points.stride = theMesh->getVertexBuffer()->getStride();
        convexDesc.points.data = theMesh->m_vertices.data();
        convexDesc.flags = physx::PxConvexFlag::eCOMPUTE_CONVEX | physx::PxConvexFlag::eDISABLE_MESH_VALIDATION | physx::PxConvexFlag::eFAST_INERTIA_COMPUTATION;

        convexDesc.indices.count = theMesh->getIndicesCount();
        convexDesc.indices.stride = theMesh->getIndexBuffer()->getStride();
        convexDesc.indices.data = theMesh->m_indices.data();

        physx::PxDefaultMemoryOutputStream buf;
        physx::PxConvexMeshCookingResult::Enum result;
        if (!m_cooking->cookConvexMesh(convexDesc, buf, &result))
            return NULL;
        physx::PxDefaultMemoryInputData input(buf.getData(), buf.getSize());
        return m_physics->createConvexMesh(input);
    }

    bool PhysX::Advance(float timePoint) {
        float advance = timePoint - m_time;
        if (advance <= 0.0f) {
            return false;
        }
        m_scene->simulate(advance);
        m_time = timePoint;
        return true;
    }

    bool PhysX::FetchResults() {
        return m_scene->fetchResults();
    }

    void PhysX::SetDebugVisual(PhsicsDebugViuals option, float value) const {
        if(m_scene)
            m_scene->setVisualizationParameter((physx::PxVisualizationParameter::Enum)option, value);
    }

    float PhysX::GetDebugVisual(PhsicsDebugViuals option) const {
        if (m_scene)
            return m_scene->getVisualizationParameter((physx::PxVisualizationParameter::Enum)option);
        return 0.0f;
    }

    void PhysX::RenderSceneDebugData() const {
        if (!m_scene)
            return;

        const auto& renderBuffer = m_scene->getRenderBuffer();
        
        
        for (size_t i = 0; i < renderBuffer.getNbLines(); i++) {
            const physx::PxDebugLine& line = renderBuffer.getLines()[i];
            Renderer2D::drawLine(PhysXToVec3(line.pos0), PhysXToVec3(line.pos1), ColorConvert(line.color0), ColorConvert(line.color1));
        }  
        
        for (size_t i = 0; i < renderBuffer.getNbPoints(); i++) {
            const physx::PxDebugPoint& point = renderBuffer.getPoints()[i];
            Gizmo::drawSphere(Math::createMat4(PhysXToVec3(point.pos), glm::vec3(0.1f)), ColorConvert(point.color));
        }

        for (size_t i = 0; i < renderBuffer.getNbTexts(); i++) {
            const physx::PxDebugText& text = renderBuffer.getTexts()[i];
            Gizmo::drawText(text.string, PhysXToVec3(text.position), ColorConvert(text.color));
        }
    }
}