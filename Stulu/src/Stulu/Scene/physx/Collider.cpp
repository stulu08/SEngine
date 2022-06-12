#include "st_pch.h"
#include "Collider.h"

#define PX_PHYSX_STATIC_LIB
#include "PxConfig.h"
#include "physx/PxPhysicsAPI.h"

#include "Stulu/Scene/GameObject.h"
#include "Stulu/Scene/Components/Components.h"


namespace Stulu {
	void Collider::destroy() {
		if (rigidbody && shape) {
			physx::PxShape* shape = (physx::PxShape*)this->shape;
			physx::PxRigidActor* actor = (physx::PxRigidActor*)rigidbody;
			actor->detachShape(*shape);
		}

		shape = nullptr;
		rigidbody = nullptr;
	}

	void BoxColliderComponent::create(GameObject object, PhysX& physics) {
		auto& tc = object.getComponent<TransformComponent>();
		object.getScene()->updateTransform(tc);
		auto& bCollide = object.getComponent<BoxColliderComponent>();
		physx::PxRigidActor* actor;
		if (object.hasComponent<RigidbodyComponent>()) {
			if (object.getComponent<RigidbodyComponent>().body)
				actor = (physx::PxRigidActor*)object.getComponent<RigidbodyComponent>().body;
			else
				actor = physics.createActor(object.getComponent<RigidbodyComponent>(), tc.worldPosition, tc.worldRotation);
		}
		else {
			RigidbodyComponent& rb = object.addComponent<RigidbodyComponent>();
			rb.useGravity = true;
			rb.kinematic = true;
			actor = physics.createActor(object.getComponent<RigidbodyComponent>(), tc.worldPosition, tc.worldRotation);
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
		object.getScene()->updateTransform(tc);
		auto& bCollide = object.getComponent<SphereColliderComponent>();

		physx::PxRigidActor* actor;
		if (object.hasComponent<RigidbodyComponent>()) {
			if (object.getComponent<RigidbodyComponent>().body)
				actor = (physx::PxRigidActor*)object.getComponent<RigidbodyComponent>().body;
			else
				actor = physics.createActor(object.getComponent<RigidbodyComponent>(), tc.worldPosition, tc.worldRotation);
		}
		else {
			RigidbodyComponent& rb = object.addComponent<RigidbodyComponent>();
			rb.useGravity = true;
			rb.kinematic = true;
			actor = physics.createActor(object.getComponent<RigidbodyComponent>(), tc.worldPosition, tc.worldRotation);
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
		object.getScene()->updateTransform(tc);
		auto& bCollide = object.getComponent<MeshColliderComponent>();

		if (!object.hasComponent<MeshFilterComponent>())
			return;
		if (bCollide.convexMesh == nullptr && bCollide.convex) {
			if (bCollide.mesh.mesh->getVerticesCount() > 255 * 3) {
				bCollide.convexMesh = createRef<Mesh>(Mesh::copyAndLimit(bCollide.mesh.mesh, 255 * 3));
			}
			else {
				bCollide.convexMesh = bCollide.mesh.mesh;
			}
		}
		if (bCollide.mesh.mesh == nullptr || (bCollide.convex && bCollide.convexMesh == nullptr))
			return;
		physx::PxRigidActor* actor;
		if (bCollide.convex) {
			actor = physics.getPhysics()->createRigidStatic(physx::PxTransform(PhysicsVec3fromglmVec3(tc.worldPosition), PhysicsQuatfromglmQuat(tc.worldRotation)));
			if (object.hasComponent<RigidbodyComponent>()) {
				actor = physics.createActor(object.getComponent<RigidbodyComponent>(), tc.worldPosition, tc.worldRotation);
			}
			else {
				RigidbodyComponent& rb = object.addComponent<RigidbodyComponent>();
				rb.useGravity = true;
				rb.kinematic = true;
				actor = physics.createActor(object.getComponent<RigidbodyComponent>(), tc.worldPosition, tc.worldRotation);
			}
		}
		else {
			actor = physics.getPhysics()->createRigidStatic(physx::PxTransform(PhysicsVec3fromglmVec3(tc.worldPosition), PhysicsQuatfromglmQuat(tc.worldRotation)));
		}
		physx::PxMaterial* aMaterial = physics.getPhysics()->createMaterial(bCollide.staticFriction, bCollide.dynamicFriction, bCollide.restitution);
		physx::PxShape* shape;
		if (!convex) {
			physx::PxTriangleMesh* physxMesh = physics.createTriangleMesh(bCollide.mesh.mesh);
			if (physxMesh) {
				shape = physx::PxRigidActorExt::createExclusiveShape(*actor, physx::PxTriangleMeshGeometry(physxMesh,
					physx::PxMeshScale(PhysicsVec3fromglmVec3(tc.worldScale))), *aMaterial);
			}
			else {
				return;
			}
		}
		else {
			physx::PxConvexMesh* physxMesh = physics.createConvexMesh(bCollide.convexMesh);
			if (physxMesh) {
				shape = physx::PxRigidActorExt::createExclusiveShape(*actor, physx::PxConvexMeshGeometry(physxMesh,
					physx::PxMeshScale(PhysicsVec3fromglmVec3(tc.worldScale))), *aMaterial);
			}
			else {
				return;
			}

		}
		physics.getScene()->addActor(*actor);
		bCollide.shape = (void*)shape;
		bCollide.rigidbody = (void*)actor;
	}
	void CapsuleColliderComponent::create(GameObject object, PhysX& physics){
		auto& tc = object.getComponent<TransformComponent>();
		object.getScene()->updateTransform(tc);
		auto& bCollide = object.getComponent<CapsuleColliderComponent>();

		physx::PxRigidActor* actor;
		if (object.hasComponent<RigidbodyComponent>()) {
			if (object.getComponent<RigidbodyComponent>().body)
				actor = (physx::PxRigidActor*)object.getComponent<RigidbodyComponent>().body;
			else
				actor = physics.createActor(object.getComponent<RigidbodyComponent>(), tc.worldPosition, tc.worldRotation);
		}
		else {
			RigidbodyComponent& rb = object.addComponent<RigidbodyComponent>();
			rb.useGravity = true;
			rb.kinematic = true;
			actor = physics.createActor(object.getComponent<RigidbodyComponent>(), tc.worldPosition, tc.worldRotation);
		}
		physx::PxTransform relativePose(PhysicsVec3fromglmVec3(bCollide.offset));
		physx::PxMaterial* aMaterial = physics.getPhysics()->createMaterial(bCollide.staticFriction, bCollide.dynamicFriction, bCollide.restitution);

		physx::PxCapsuleGeometry geometry;
		if(horizontal)
			geometry = physx::PxCapsuleGeometry(tc.worldScale.x * bCollide.radius/2.0f, tc.worldScale.y * (height / 4.0f));
		else
			geometry = physx::PxCapsuleGeometry(tc.worldScale.y * bCollide.radius/2.0f, tc.worldScale.x * (height / 4.0f));

		physx::PxShape* shape = physx::PxRigidActorExt::createExclusiveShape(*actor, geometry, *aMaterial);
		shape->setLocalPose(relativePose);
		bCollide.shape = (void*)shape;
		bCollide.rigidbody = (void*)actor;
		physics.getScene()->addActor(*actor);
	}

}