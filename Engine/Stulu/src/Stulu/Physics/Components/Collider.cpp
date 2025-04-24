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
			if (actorComponent.GetActor())
				actorComponent.GetActor()->detachShape(*m_shape);
		}
		

		if (m_shape && m_shape->isReleasable())
			m_shape->release();

		if (material.GetMaterial() && material.GetMaterial()->isReleasable())
			material.GetMaterial()->release();

		m_shape = nullptr;
	}

	void Collider::SetMaterial(PhysicsMaterial& newMaterial) {
		if (m_shape) {
			newMaterial.CreateMaterial();
			physx::PxMaterial* mat = newMaterial.GetMaterial();
			m_shape->setMaterials(&mat, 1);

			if (material.GetMaterial() && material.GetMaterial()->isReleasable())
				material.GetMaterial()->release();
		}

		material = newMaterial;

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

		material.CreateMaterial();

		m_shape = physx::PxRigidActorExt::createExclusiveShape(
			*actorComponent.GetActor(), 
			physx::PxBoxGeometry(PhysicsHelper::Vec3ToPhysX(transform.GetWorldScale() * Size)), *material.GetMaterial());

		m_shape->setLocalPose(physx::PxTransform(PhysicsHelper::Vec3ToPhysX(Offset)));

		if (HasRigidBody()) {
			GetRigidBody().WakeUp();
		}
	}
	void BoxColliderComponent::SetOffset(glm::vec3 value) {
		Offset = value;
		if (m_shape) {
			m_shape->setLocalPose(physx::PxTransform(PhysicsHelper::Vec3ToPhysX(Offset)));
		}

	}
	void BoxColliderComponent::SetSize(glm::vec3 value) {
		Size = value;
		if (m_shape) {
			const auto& transform = gameObject.getComponent<TransformComponent>();
			m_shape->setGeometry(physx::PxBoxGeometry(PhysicsHelper::Vec3ToPhysX(transform.GetWorldScale() * Size)));
		}
	}

	void SphereColliderComponent::Create(PhysicsScene* physics) {
		if (!physics->IsValid())
			return;
		m_physics = physics;
		CreateActor();
		
		RigidActorComponent& actorComponent = GetActor();
		const auto& transform = gameObject.getComponent<TransformComponent>();

		material.CreateMaterial();

		m_shape = physx::PxRigidActorExt::createExclusiveShape(
			*actorComponent.GetActor(),
			physx::PxSphereGeometry(transform.GetWorldScale().x * Radius), *material.GetMaterial());

		m_shape->setLocalPose(physx::PxTransform(PhysicsHelper::Vec3ToPhysX(Offset)));

		if (HasRigidBody()) {
			GetRigidBody().WakeUp();
		}
	}
	void SphereColliderComponent::SetOffset(glm::vec3 value) {
		Offset = value;
		if (m_shape) {
			m_shape->setLocalPose(physx::PxTransform(PhysicsHelper::Vec3ToPhysX(Offset)));
		}

	}
	void SphereColliderComponent::SetRadius(float value) {
		Radius = value;
		if (m_shape) {
			const auto& transform = gameObject.getComponent<TransformComponent>();
			m_shape->setGeometry(physx::PxSphereGeometry(transform.GetWorldScale().x * Radius));
		}
	}

	physx::PxCapsuleGeometry CreateCapsule(bool Vertical, float Radius, float Height, const glm::vec3& scale) {
		float scaledRadius = Radius * ((Vertical) ? scale.z : scale.x);
		float scaledHalfHeight = ((Height * 0.5f) - Radius) * ((Vertical) ? scale.x : scale.y);
		return physx::PxCapsuleGeometry(scaledRadius, scaledHalfHeight);
	}
	void CapsuleColliderComponent::Create(PhysicsScene* physics) {
		if (!physics->IsValid())
			return;
		m_physics = physics;
		CreateActor();

		RigidActorComponent& actorComponent = GetActor();
		const auto& transform = gameObject.getComponent<TransformComponent>();

		material.CreateMaterial();

		m_shape = physx::PxRigidActorExt::createExclusiveShape(
			*actorComponent.GetActor(),
			CreateCapsule(Vertical, Radius, Height, transform.GetWorldScale()), *material.GetMaterial());

		physx::PxQuat rotation = Vertical ? physx::PxQuat(physx::PxHalfPi, physx::PxVec3(0, 0, 1)) : physx::PxQuat(physx::PxIdentity);
		m_shape->setLocalPose(physx::PxTransform(PhysicsHelper::Vec3ToPhysX(Offset), rotation));

		if (HasRigidBody()) {
			GetRigidBody().WakeUp();
		}
	}
	void CapsuleColliderComponent::SetOffset(glm::vec3 value) {
		Offset = value;
		if (m_shape) {
			physx::PxQuat rotation = Vertical ? physx::PxQuat(physx::PxHalfPi, physx::PxVec3(0, 0, 1)) : physx::PxQuat(physx::PxIdentity);
			m_shape->setLocalPose(physx::PxTransform(PhysicsHelper::Vec3ToPhysX(Offset), rotation));
		}

	}
	void CapsuleColliderComponent::SetVertical(bool value) {
		Vertical = value;
		if (m_shape) {
			const auto& transform = gameObject.getComponent<TransformComponent>();
			physx::PxQuat rotation = Vertical ? physx::PxQuat(physx::PxHalfPi, physx::PxVec3(0, 0, 1)) : physx::PxQuat(physx::PxIdentity);
			m_shape->setLocalPose(physx::PxTransform(PhysicsHelper::Vec3ToPhysX(Offset), rotation));
			m_shape->setGeometry(CreateCapsule(Vertical, Radius, Height, transform.GetWorldScale()));
		}
	}
	void CapsuleColliderComponent::SetRadius(float value) {
		Radius = glm::max(0.0000001f, value);
		if (m_shape) {
			const auto& transform = gameObject.getComponent<TransformComponent>();
			m_shape->setGeometry(CreateCapsule(Vertical, Radius, Height, transform.GetWorldScale()));
		}
	}
	void CapsuleColliderComponent::SetHeight(float value) {
		Height = glm::max(0.0000001f, value);

		if (m_shape) {
			const auto& transform = gameObject.getComponent<TransformComponent>();
			m_shape->setGeometry(CreateCapsule(Vertical, Radius, Height, transform.GetWorldScale()));
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
		material.CreateMaterial();

		if (!Mesh.IsValid()) {
			CORE_WARN("No mesh specified for Mesh Collider");
			return;
		}

		if (Convex) {
			physx::PxConvexMesh* physxMesh = PhysicsHelper::CreateConvexMesh(*Mesh);
			m_shape = physx::PxRigidActorExt::createExclusiveShape(
				*actorComponent.GetActor(),
				physx::PxConvexMeshGeometry(physxMesh, physx::PxMeshScale(PhysicsHelper::Vec3ToPhysX(transform.GetWorldScale()))),
				*material.GetMaterial());

			if (HasRigidBody()) {
				GetRigidBody().WakeUp();
			}
		}
		else {
			if (HasRigidBody()) {
				CORE_ERROR("Cannot use a rigidbody on a static mesh, use a convex mesh instead");
				return;
			}
			physx::PxTriangleMesh* physxMesh = PhysicsHelper::CreateTriangleMesh(*Mesh);
			m_shape = physx::PxRigidActorExt::createExclusiveShape(
				*actorComponent.GetActor(),
				physx::PxTriangleMeshGeometry(physxMesh, physx::PxMeshScale(PhysicsHelper::Vec3ToPhysX(transform.GetWorldScale()))),
				*material.GetMaterial());
		}
	}
	void MeshColliderComponent::SetMesh(const MeshAsset& value) {
		Mesh = value;
		if (m_shape) {
			const auto& transform = gameObject.getComponent<TransformComponent>();
			
			if (!Mesh.IsValid()) {
				CORE_WARN("No mesh specified for Mesh Collider");
				return;
			}

			if (Convex) {
				physx::PxConvexMesh* physxMesh = PhysicsHelper::CreateConvexMesh(*Mesh);
				m_shape->setGeometry(physx::PxConvexMeshGeometry(physxMesh, physx::PxMeshScale(PhysicsHelper::Vec3ToPhysX(transform.GetWorldScale()))));
				
			}
			else {
				if (HasRigidBody()) {
					CORE_ERROR("Cannot use a rigidbody on a static mesh, use a convex mesh instead");
					return;
				}
				physx::PxTriangleMesh* physxMesh = PhysicsHelper::CreateTriangleMesh(*Mesh);
				m_shape->setGeometry(physx::PxTriangleMeshGeometry(physxMesh, physx::PxMeshScale(PhysicsHelper::Vec3ToPhysX(transform.GetWorldScale()))));
			}
		}
	}
}