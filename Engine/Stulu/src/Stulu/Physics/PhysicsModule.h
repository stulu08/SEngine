#pragma once
#include <Stulu/Core/Module.h>
#include "PhysicsHelper.h"

namespace Stulu {
	class PhysicsModule : public Module {
	public:
		using ModuleType = ModuleDefault;

		PhysicsModule();
		~PhysicsModule();

		bool onLoad(const ApplicationInfo& appInfo) override;
		void onDetach() override;
		void onAttach() override;

		void LoadPhysics();
		void LoadBindings();

		static inline bool PhysicsEnabled() { return s_instance != nullptr; }
		static inline PhysicsModule& Get() { return *s_instance; }

		inline physx::PxFoundation* GetFoundation() const { return m_foundation; };
		inline physx::PxCudaContextManager* GetCudaContext() const { return m_cudaContextManager; };
		inline physx::PxPhysics* GetPhysics() const { return m_physics; };
		inline physx::PxCpuDispatcher* GetCpuDispatcher() const { return m_cpuDispatcher; };
		inline physx::PxCooking* GetCooking() const { return m_cooking; };
		inline bool CudaSupport() const { return m_cudaContextManager != nullptr; }
	private:
		physx::PxFoundation* m_foundation = nullptr;
		physx::PxCudaContextManager* m_cudaContextManager = nullptr;
		physx::PxPhysics* m_physics = nullptr;
		physx::PxCpuDispatcher* m_cpuDispatcher = nullptr;
		physx::PxCooking* m_cooking = nullptr;


		static PhysicsModule* s_instance;
	};
}