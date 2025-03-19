#include "st_pch.h"
#include "Collider.h"

#define PX_PHYSX_STATIC_LIB
#include "PxConfig.h"
#include "PxPhysicsAPI.h"

#include "Stulu/Scene/GameObject.h"
#include "Stulu/Scene/Components/Components.h"


namespace Stulu {
	void Collider::Release() {
		if (!gameObject)
			return;

		if (HasActorAttached()) {
			const auto& actorComponent = GetActor();
			if(actorComponent.GetActor())
				actorComponent.GetActor()->detachShape(*m_shape);
		}
		

		if (m_shape && m_shape->isReleasable())
			m_shape->release();

		if (PhysicsMaterial.GetMaterial() && PhysicsMaterial.GetMaterial()->isReleasable())
			PhysicsMaterial.GetMaterial()->release();

		m_shape = nullptr;
	}

	inline bool Collider::HasActorAttached() const {
		return gameObject.hasComponent<RigidActorComponent>();
	}

	inline RigidActorComponent& Collider::GetActor() const {
		return gameObject.getComponent<RigidActorComponent>();
	}

	void Collider::SetDebugVisuals(bool value) const {
		if (m_physics->IsValid() && m_shape) {
			m_shape->setFlag(physx::PxShapeFlag::eVISUALIZATION, value);
		}
	}

	void Collider::CreateActor() {
		if (gameObject.hasComponent<RigidActorComponent>()) {
			RigidActorComponent& actorComponent = gameObject.getComponent<RigidActorComponent>();
			if (!actorComponent.RuntimeValid()) {
				actorComponent.Create(m_physics);
			}
		}
		else {
			RigidActorComponent& actorComponent = gameObject.addComponent<RigidStaticComponent>();
			if (!actorComponent.RuntimeValid()) {
				actorComponent.Create(m_physics);
			}
		}
	}

	void BoxColliderComponent::Create(PhysicsScene* physics) {
		if (!physics->IsValid())
			return;
		m_physics = physics;
		CreateActor();

		RigidActorComponent& actorComponent = GetActor();
		const auto& transform = gameObject.getComponent<TransformComponent>();

		PhysicsMaterial.CreateMaterial();

		m_shape = physx::PxRigidActorExt::createExclusiveShape(
			*actorComponent.GetActor(), 
			physx::PxBoxGeometry(PhysicsHelper::Vec3ToPhysX(transform.GetWorldScale() * Size)), *PhysicsMaterial.GetMaterial());

		m_shape->setLocalPose(physx::PxTransform(PhysicsHelper::Vec3ToPhysX(Offset)));

		if (HasRigidBody()) {
			GetRigidBody().WakeUp();
		}
	}
	void SphereColliderComponent::Create(PhysicsScene* physics) {
		if (!physics->IsValid())
			return;
		m_physics = physics;
		CreateActor();
		
		RigidActorComponent& actorComponent = GetActor();
		const auto& transform = gameObject.getComponent<TransformComponent>();

		PhysicsMaterial.CreateMaterial();

		m_shape = physx::PxRigidActorExt::createExclusiveShape(
			*actorComponent.GetActor(),
			physx::PxSphereGeometry(transform.GetWorldScale().x * Radius), *PhysicsMaterial.GetMaterial());

		m_shape->setLocalPose(physx::PxTransform(PhysicsHelper::Vec3ToPhysX(Offset)));

		if (HasRigidBody()) {
			GetRigidBody().WakeUp();
		}
	}
	void CapsuleColliderComponent::Create(PhysicsScene* physics) {
		if (!physics->IsValid())
			return;
		m_physics = physics;
		CreateActor();

		RigidActorComponent& actorComponent = GetActor();
		const auto& transform = gameObject.getComponent<TransformComponent>();

		PhysicsMaterial.CreateMaterial();

		physx::PxCapsuleGeometry geometry;
		if (Horizontal)
			geometry = physx::PxCapsuleGeometry(transform.GetWorldScale().x * Radius / 2.0f, transform.GetWorldScale().y * (Height / 4.0f));
		else
			geometry = physx::PxCapsuleGeometry(transform.GetWorldScale().y * Radius / 2.0f, transform.GetWorldScale().x * (Height / 4.0f));

		m_shape = physx::PxRigidActorExt::createExclusiveShape(
			*actorComponent.GetActor(),
			geometry, *PhysicsMaterial.GetMaterial());

		m_shape->setLocalPose(physx::PxTransform(PhysicsHelper::Vec3ToPhysX(Offset)));

		if (HasRigidBody()) {
			GetRigidBody().WakeUp();
		}
	}
	void MeshColliderComponent::Create(PhysicsScene* physics) {
		if (!physics->IsValid())
			return;

		m_physics = physics;

		if (!gameObject.hasComponent<MeshFilterComponent>()) {
			CORE_WARN("No mesh specified for Mesh Collider")
			return;
		}

		CreateActor();
		RigidActorComponent& actorComponent = GetActor();
		const auto& transform = gameObject.getComponent<TransformComponent>();
		PhysicsMaterial.CreateMaterial();

		if (Convex) {
			if (ConvexMesh == nullptr) {
				BuildConvex();
			}

			physx::PxConvexMesh* physxMesh = PhysicsHelper::CreateConvexMesh(ConvexMesh);
			m_shape = physx::PxRigidActorExt::createExclusiveShape(
				*actorComponent.GetActor(),
				physx::PxConvexMeshGeometry(physxMesh, physx::PxMeshScale(PhysicsHelper::Vec3ToPhysX(transform.GetWorldScale()))),
				*PhysicsMaterial.GetMaterial());

			if (HasRigidBody()) {
				GetRigidBody().WakeUp();
			}
		}
		else {
			if (Mesh.mesh == nullptr) {
				CORE_WARN("No mesh specified for Mesh Collider");
				return;
			}
			if (HasRigidBody()) {
				CORE_ERROR("Cannot use a rigidbody on a static mesh, use a convex mesh instead");
				return;
			}
			physx::PxTriangleMesh* physxMesh = PhysicsHelper::CreateTriangleMesh(Mesh.mesh);
			m_shape = physx::PxRigidActorExt::createExclusiveShape(
				*actorComponent.GetActor(),
				physx::PxTriangleMeshGeometry(physxMesh, physx::PxMeshScale(PhysicsHelper::Vec3ToPhysX(transform.GetWorldScale()))),
				*PhysicsMaterial.GetMaterial());
		}
	}
	void MeshColliderComponent::BuildConvex() {
		constexpr size_t maxVertices = static_cast<uint64_t>(255) * 3;
		if (Mesh.mesh) {
			if (Mesh.mesh->getVerticesCount() > maxVertices) {
				ConvexMesh = createRef<Stulu::Mesh>(Mesh::copyAndLimit(Mesh.mesh, maxVertices));
			}
			else {
				ConvexMesh = Mesh.mesh;
			}
			return;
		}
		CORE_WARN("No mesh specified for Mesh Collider");

	}
}