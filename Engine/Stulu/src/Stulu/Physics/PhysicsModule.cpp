#include "st_pch.h"
#include "PhysicsModule.h"

#include "PhysicsScene.h"
#include "Stulu/Scripting/EventCaller.h"

#include "Components/RigidbodyComponent.h"
#include "Components/Collider.h"

#define PX_PHYSX_STATIC_LIB
#include "PxPhysicsAPI.h"

#include "Bindings/Rigidbody.h"
#include "Bindings/Collider.h"

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

    class PhysicsTask : public Task {
    public:
        PhysicsTask(physx::PxBaseTask* task)
            : m_task(task) {
            if (m_task) {
                Assign();
            }
        }
        virtual ~PhysicsTask() {
            m_task->release();
        }

        virtual void Run() override {
            m_task->run();
        }

        virtual const char* GetName() const override {
            return m_task->getName();
        }
    private:
        physx::PxBaseTask* m_task;
    };

    class PhysicsCPUDispatcher : public physx::PxCpuDispatcher {
    public:
        PhysicsCPUDispatcher() {
            m_dispatcher = &(Application::get().GetCpuDispatcher());
        }
        void submitTask(physx::PxBaseTask& task) override {
            m_dispatcher->DispatchTask<PhysicsTask>(&task);
        }
        uint32_t getWorkerCount() const override {
            return static_cast<uint32_t>(m_dispatcher->ThreadCount());
        }
    private:
        CpuDispatcher* m_dispatcher;
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
            delete m_cpuDispatcher;
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

        m_cpuDispatcher = new PhysicsCPUDispatcher();
        if (!m_cpuDispatcher) {
            CORE_ERROR("CpuDispatcher Creation failed!");
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
        manager->RegisterFunction("Stulu.InternalCalls::rigidbody_massCenterGet(ulong,Stulu.Vector3&)", StuluBindings::Rigidbody::getMassCenterPos);
        manager->RegisterFunction("Stulu.InternalCalls::rigidbody_lockAngularVelocity(ulong,int)", StuluBindings::Rigidbody::getAngularLock);
        manager->RegisterFunction("Stulu.InternalCalls::rigidbody_lockAngularVelocity(ulong,int,bool)", StuluBindings::Rigidbody::setAngularLock);
        manager->RegisterFunction("Stulu.InternalCalls::rigidbody_lockLinearVelocity(ulong,int)", StuluBindings::Rigidbody::getLinearLock);
        manager->RegisterFunction("Stulu.InternalCalls::rigidbody_lockLinearVelocity(ulong,int,bool)", StuluBindings::Rigidbody::setLinearLock);

		manager->RegisterFunction("Stulu.InternalCalls::boxcollider_getMaterial(ulong,Stulu.PhysicsMaterial&)", StuluBindings::BoxCollider::getMaterial);
		manager->RegisterFunction("Stulu.InternalCalls::boxcollider_updateMaterial(ulong,Stulu.PhysicsMaterial&)", StuluBindings::BoxCollider::setMaterial);
		manager->RegisterFunction("Stulu.InternalCalls::boxcollider_getOffset(ulong,Stulu.Vector3&)", StuluBindings::BoxCollider::getOffset);
		manager->RegisterFunction("Stulu.InternalCalls::boxcollider_setOffset(ulong,Stulu.Vector3&)", StuluBindings::BoxCollider::setOffset);
		manager->RegisterFunction("Stulu.InternalCalls::boxcollider_getSize(ulong,Stulu.Vector3&)", StuluBindings::BoxCollider::getSize);
		manager->RegisterFunction("Stulu.InternalCalls::boxcollider_setSize(ulong,Stulu.Vector3&)", StuluBindings::BoxCollider::setSize);
		
        manager->RegisterFunction("Stulu.InternalCalls::spherecollider_getMaterial(ulong,Stulu.PhysicsMaterial&)", StuluBindings::SphereCollider::getMaterial);
		manager->RegisterFunction("Stulu.InternalCalls::spherecollider_updateMaterial(ulong,Stulu.PhysicsMaterial&)", StuluBindings::SphereCollider::setMaterial);
		manager->RegisterFunction("Stulu.InternalCalls::spherecollider_getOffset(ulong,Stulu.Vector3&)", StuluBindings::SphereCollider::getOffset);
		manager->RegisterFunction("Stulu.InternalCalls::spherecollider_setOffset(ulong,Stulu.Vector3&)", StuluBindings::SphereCollider::setOffset);
		manager->RegisterFunction("Stulu.InternalCalls::spherecollider_radius(ulong)", StuluBindings::SphereCollider::getRadius);
		manager->RegisterFunction("Stulu.InternalCalls::spherecollider_radius(ulong,single)", StuluBindings::SphereCollider::setRadius);
		
        manager->RegisterFunction("Stulu.InternalCalls::capsuleCollider_getMaterial(ulong,Stulu.PhysicsMaterial&)", StuluBindings::CapsuleCollider::getMaterial);
		manager->RegisterFunction("Stulu.InternalCalls::capsuleCollider_updateMaterial(ulong,Stulu.PhysicsMaterial&)", StuluBindings::CapsuleCollider::setMaterial);
		manager->RegisterFunction("Stulu.InternalCalls::capsuleCollider_getOffset(ulong,Stulu.Vector3&)", StuluBindings::CapsuleCollider::getOffset);
		manager->RegisterFunction("Stulu.InternalCalls::capsuleCollider_setOffset(ulong,Stulu.Vector3&)", StuluBindings::CapsuleCollider::setOffset);
		manager->RegisterFunction("Stulu.InternalCalls::capsuleCollider_radius(ulong)", StuluBindings::CapsuleCollider::getRadius);
		manager->RegisterFunction("Stulu.InternalCalls::capsuleCollider_radius(ulong,single)", StuluBindings::CapsuleCollider::setRadius);
		manager->RegisterFunction("Stulu.InternalCalls::capsuleCollider_height(ulong)", StuluBindings::CapsuleCollider::getHeight);
		manager->RegisterFunction("Stulu.InternalCalls::capsuleCollider_height(ulong,single)", StuluBindings::CapsuleCollider::setHeight);
		manager->RegisterFunction("Stulu.InternalCalls::capsuleCollider_vertical(ulong)", StuluBindings::CapsuleCollider::getVertical);
		manager->RegisterFunction("Stulu.InternalCalls::capsuleCollider_vertical(ulong,bool)", StuluBindings::CapsuleCollider::setVertical);
    }
}