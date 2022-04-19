#include "st_pch.h"
#include "Collider.h"

#define PX_PHYSX_STATIC_LIB
#include "PxConfig.h"
#include "physx/PxPhysicsAPI.h"

#include "Stulu/Scene/GameObject.h"
#include "Stulu/Scene/Components/Components.h"


namespace Stulu {
	void Collider::destroy(GameObject object, PhysX& physics) {
		if (localRigidbody && rigidbody) {
			rigidbodyComponent.destroy(object, physics);
			localRigidbody = false;
			rigidbody = nullptr;
		}
		if (shape) {
			shape = nullptr;
		}
	}

	void BoxColliderComponent::create(GameObject object, PhysX& physics) {
		auto& tc = object.getComponent<TransformComponent>();
		auto& bCollide = object.getComponent<BoxColliderComponent>();
		physx::PxRigidActor* actor;
		if (object.hasComponent<RigidbodyComponent>()) {
			actor = physics.createActor(object.getComponent<RigidbodyComponent>(), tc.worldPosition, tc.worldRotation);
			rigidbodyComponent = object.getComponent<RigidbodyComponent>();
		}
		else {
			RigidbodyComponent& rb = object.addComponent<RigidbodyComponent>();
			rb.useGravity = true;
			rb.kinematic = true;
			actor = physics.createActor(object.getComponent<RigidbodyComponent>(), tc.worldPosition, tc.worldRotation);
			rigidbodyComponent = rb;
		}
		physx::PxTransform relativePose(PhysicsVec3fromglmVec3(bCollide.offset));
		physx::PxMaterial* aMaterial = physics.getPhysics()->createMaterial(bCollide.staticFriction, bCollide.dynamicFriction, bCollide.restitution);
		physx::PxShape* shape = physx::PxRigidActorExt::createExclusiveShape(*actor, physx::PxBoxGeometry(PhysicsVec3fromglmVec3(tc.worldScale * bCollide.size)), *aMaterial);
		shape->setLocalPose(relativePose);

		bCollide.shape = (void*)shape;
		bCollide.rigidbody = (void*)actor;
		physics.getScene()->addActor(*actor);
	}
	void SphereColliderComponent::create(GameObject object, PhysX& physics) {
		auto& tc = object.getComponent<TransformComponent>();
		auto& bCollide = object.getComponent<SphereColliderComponent>();

		physx::PxRigidActor* actor;
		if (object.hasComponent<RigidbodyComponent>()) {
			actor = physics.createActor(object.getComponent<RigidbodyComponent>(), tc.worldPosition, tc.worldRotation);
			rigidbodyComponent = object.getComponent<RigidbodyComponent>();
		}
		else {
			RigidbodyComponent& rb = object.addComponent<RigidbodyComponent>();
			rb.useGravity = true;
			rb.kinematic = true;
			actor = physics.createActor(object.getComponent<RigidbodyComponent>(), tc.worldPosition, tc.worldRotation);
			rigidbodyComponent = rb;
		}
		physx::PxTransform relativePose(PhysicsVec3fromglmVec3(bCollide.offset));
		physx::PxMaterial* aMaterial = physics.getPhysics()->createMaterial(bCollide.staticFriction, bCollide.dynamicFriction, bCollide.restitution);
		physx::PxShape* shape = physx::PxRigidActorExt::createExclusiveShape(*actor, physx::PxSphereGeometry(tc.worldScale.x * bCollide.radius), *aMaterial);
		shape->setLocalPose(relativePose);
		bCollide.shape = (void*)shape;
		bCollide.rigidbody = (void*)actor;
		physics.getScene()->addActor(*actor);
	}
	void MeshColliderComponent::create(GameObject object, PhysX& physics) {
		auto& tc = object.getComponent<TransformComponent>();
		auto& bCollide = object.getComponent<MeshColliderComponent>();

		if (!object.hasComponent<MeshFilterComponent>())
			return;
		Ref<Mesh>& mesh = object.getComponent<MeshFilterComponent>().mesh.mesh;
		if (mesh == nullptr)
			return;

		physx::PxRigidActor* actor = physics.getPhysics()->createRigidStatic(physx::PxTransform(PhysicsVec3fromglmVec3(tc.worldPosition), PhysicsQuatfromglmQuat(tc.worldRotation)));
		rigidbodyComponent = RigidbodyComponent((void*)actor);

		physx::PxMaterial* aMaterial = physics.getPhysics()->createMaterial(bCollide.staticFriction, bCollide.dynamicFriction, bCollide.restitution);
		physx::PxShape* shape = physx::PxRigidActorExt::createExclusiveShape(*actor, physx::PxTriangleMeshGeometry(physics.createTriangleMesh(mesh),
			physx::PxMeshScale(PhysicsVec3fromglmVec3(tc.worldScale))), *aMaterial);

		physics.getScene()->addActor(*actor);
		bCollide.shape = (void*)shape;
		bCollide.rigidbody = (void*)actor;
	}
}