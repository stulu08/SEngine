#include "st_pch.h"
#include "PhysicsModule.h"

#include "PhysicsScene.h"
#include "Stulu/Scripting/EventCaller.h"

#include "Components/RigidbodyComponent.h"
#include "Components/Collider.h"

#define PX_PHYSX_STATIC_LIB
#include "PxPhysicsAPI.h"

#include "Bindings/Rigidbody.h"

namespace Stulu {
    PhysicsModule* PhysicsModule::s_instance;

    class StDefaultAllocator : public physx::PxAllocatorCallback {
    public:
        void* allocate(size_t size, const char*, const char*, int) {
            void* ptr = physx::platformAlignedAlloc(size);
            PX_ASSERT((reinterpret_cast<size_t>(ptr) & 15) == 0);
            return ptr;
        }

        void deallocate(void* ptr) {
            physx::platformAlignedFree(ptr);
        }
    };

    class SnippetGpuLoadHook : public PxGpuLoadHook {
        virtual const char* getPhysXGpuDllName() const {
            return "Data/PhysX/PhysXGpu_64.dll";
        }
    } gGpuLoadHook;

    class UserErrorCallback : public physx::PxErrorCallback {
    public:
        virtual void reportError(physx::PxErrorCode::Enum code, const char* message, const char* file, int line) {
            switch (code) {
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


	PhysicsModule::PhysicsModule()
		: Module("PhysicsModule") {
        s_instance = this;
	}
	PhysicsModule::~PhysicsModule() {
        s_instance = nullptr;
	}

    void PhysicsModule::onLoad() {
        EventCaller::RegisterLayer<PhysicsScene>();
    }

    void PhysicsModule::onDetach() {
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
        if (m_cudaContextManager) {
            m_cudaContextManager->releaseContext();
            m_cudaContextManager->release();
            m_cudaContextManager = nullptr;
        }
        if (m_foundation) {
            m_foundation->release();
            m_foundation = nullptr;
        }
	}
	void PhysicsModule::onAttach() {
        LoadPhysics();
        LoadBindings();
	}
    void PhysicsModule::LoadPhysics() {
        static UserErrorCallback gDefaultErrorCallback;
        static StDefaultAllocator gDefaultAllocatorCallback;

        m_foundation = PxCreateFoundation(PX_PHYSICS_VERSION, gDefaultAllocatorCallback, gDefaultErrorCallback);
        if (!m_foundation) {
            CORE_ERROR("PxCreateFoundation failed!");
            return;
        }

        // Optional: Enable GPU load hook only if GPU support is required
        PxSetPhysXGpuLoadHook(&gGpuLoadHook);

#if PX_WINDOWS
        // Create GPU dispatcher
        physx::PxCudaContextManagerDesc cudaContextManagerDesc;
        m_cudaContextManager = PxCreateCudaContextManager(*m_foundation, cudaContextManagerDesc);
        if (!m_cudaContextManager || !m_cudaContextManager->contextIsValid()) {
            CORE_WARN("PxCreateCudaContextManager failed or is invalid!");
            m_cudaContextManager = nullptr;
        }
#endif


        physx::PxTolerancesScale tollerantScale;
        tollerantScale.length = 1.0f;
        tollerantScale.speed = 9.81f;

        m_physics = PxCreatePhysics(PX_PHYSICS_VERSION, *m_foundation, tollerantScale, false);
        if (!m_physics) {
            CORE_ERROR("PxCreatePhysics failed!");
            return;
        }

        // Only initialize extensions if needed
        if (!PxInitExtensions(*m_physics, nullptr)) {
            CORE_ERROR("PxInitExtensions failed!");
            return;
        }

        m_cooking = PxCreateCooking(PX_PHYSICS_VERSION, m_physics->getFoundation(), physx::PxCookingParams(m_physics->getTolerancesScale()));
        if (!m_cooking) {
            CORE_ERROR("PxCreateCooking failed!");
            return;
        }

        m_cpuDispatcher = physx::PxDefaultCpuDispatcherCreate(glm::max(std::thread::hardware_concurrency() / 2, 1u));
        if (!m_cpuDispatcher) {
            CORE_ERROR("PxDefaultCpuDispatcherCreate failed!");
            return;
        }
    }
    

    void PhysicsModule::LoadBindings() {
        Component::Register<RigidbodyComponent>("Stulu.RigidbodyComponent");
        Component::Register<RigidStaticComponent>("Stulu.RigidStaticComponent");
        Component::Register<BoxColliderComponent>("Stulu.BoxColliderComponent");
        Component::Register<SphereColliderComponent>("Stulu.SphereColliderComponent");
        Component::Register<CapsuleColliderComponent>("Stulu.CapsuleColliderComponent");
        Component::Register<MeshColliderComponent>("Stulu.MeshColliderComponent");

        auto& manager = Application::get().getAssemblyManager();

        manager->RegisterFunction("Stulu.InternalCalls::rigidbody_addForce(ulong,Stulu.Vector3&,uint)", StuluBindings::Rigidbody::addForce);
        manager->RegisterFunction("Stulu.InternalCalls::rigidbody_useGravity(ulong)", StuluBindings::Rigidbody::getuseGravity);
        manager->RegisterFunction("Stulu.InternalCalls::rigidbody_kinematic(ulong)", StuluBindings::Rigidbody::getKinematic);
        manager->RegisterFunction("Stulu.InternalCalls::rigidbody_retainaccel(ulong)", StuluBindings::Rigidbody::getRetainAccel);
        manager->RegisterFunction("Stulu.InternalCalls::rigidbody_mass(ulong)", StuluBindings::Rigidbody::getMass);
        manager->RegisterFunction("Stulu.InternalCalls::rigidbody_useGravity(ulong,bool)", StuluBindings::Rigidbody::setuseGravity);
        manager->RegisterFunction("Stulu.InternalCalls::rigidbody_kinematic(ulong,bool)", StuluBindings::Rigidbody::setKinematic);
        manager->RegisterFunction("Stulu.InternalCalls::rigidbody_retainaccel(ulong,bool)", StuluBindings::Rigidbody::setRetainAccel);
        manager->RegisterFunction("Stulu.InternalCalls::rigidbody_mass(ulong,single)", StuluBindings::Rigidbody::setMass);
        manager->RegisterFunction("Stulu.InternalCalls::rigidbody_massCenterSet(ulong,Stulu.Vector3&)", StuluBindings::Rigidbody::setMassCenterPos);
        manager->RegisterFunction("Stulu.InternalCalls::rigidbody_massCenterGet(ulong,Stulu.Vector3&)", StuluBindings::Rigidbody::getMassCenterPos);
    }
}