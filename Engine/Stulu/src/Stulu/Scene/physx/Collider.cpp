#include "st_pch.h"
#include "Collider.h"

#define PX_PHYSX_STATIC_LIB
#include "PxConfig.h"
#include "PxPhysicsAPI.h"

#include "Stulu/Scene/GameObject.h"
#include "Stulu/Scene/Components/Components.h"


namespace Stulu {
	void Collider::destroy() {
		if (!gameObject.getScene()->getData().enablePhsyics3D)
			return;
		if (rigidbody && shape) {
			physx::PxShape* shape = (physx::PxShape*)this->shape;
			physx::PxRigidActor* actor = (physx::PxRigidActor*)rigidbody;
			actor->detachShape(*shape);
		}

		shape = nullptr;
		rigidbody = nullptr;
	}

	void BoxColliderComponent::create() {
		auto physics = gameObject.getScene()->getPhysics();

		if (!gameObject.getScene()->getData().enablePhsyics3D)
			return;
		auto& tc = gameObject.getComponent<TransformComponent>();
		auto& bCollide = gameObject.getComponent<BoxColliderComponent>();
		physx::PxRigidActor* actor;
		if (gameObject.hasComponent<RigidbodyComponent>()) {
			RigidbodyComponent& rb = gameObject.getComponent<RigidbodyComponent>();
			if (rb.body)
				actor = (physx::PxRigidActor*)rb.body;
			else
				actor = physics->createActor(rb, tc.GetWorldPosition(), tc.GetWorldRotation());
		}
		else {
			RigidbodyComponent& rb = gameObject.addComponent<RigidbodyComponent>();
			rb.useGravity = true;
			rb.kinematic = true;
			actor = physics->createActor(rb, tc.GetWorldPosition(), tc.GetWorldRotation());
		}
		RigidbodyComponent& rb = gameObject.getComponent<RigidbodyComponent>();
		physx::PxTransform relativePose(PhysicsVec3fromglmVec3(bCollide.offset));
		physx::PxMaterial* aMaterial = physics->getPhysics()->createMaterial(bCollide.staticFriction, bCollide.dynamicFriction, bCollide.restitution);
		physx::PxShape* shape = physx::PxRigidActorExt::createExclusiveShape(*actor, physx::PxBoxGeometry(PhysicsVec3fromglmVec3(tc.GetWorldScale() * bCollide.size)), *aMaterial);
		shape->setLocalPose(relativePose);
		bCollide.shape = (void*)shape;
		bCollide.rigidbody = (void*)actor;
		rb.body = actor;
		physics->getScene()->addActor(*actor);
	}
	void SphereColliderComponent::create() {
		auto physics = gameObject.getScene()->getPhysics();

		if (!gameObject.getScene()->getData().enablePhsyics3D)
			return;
		auto& tc = gameObject.getComponent<TransformComponent>();
		auto& bCollide = gameObject.getComponent<SphereColliderComponent>();
		physx::PxRigidActor* actor;
		if (gameObject.hasComponent<RigidbodyComponent>()) {
			RigidbodyComponent& rb = gameObject.getComponent<RigidbodyComponent>();
			if (rb.body)
				actor = (physx::PxRigidActor*)rb.body;
			else
				actor = physics->createActor(rb, tc.GetWorldPosition(), tc.GetWorldRotation());
		}
		else {
			RigidbodyComponent& rb = gameObject.addComponent<RigidbodyComponent>();
			rb.useGravity = true;
			rb.kinematic = true;
			actor = physics->createActor(rb, tc.GetWorldPosition(), tc.GetWorldRotation());
		}
		RigidbodyComponent& rb = gameObject.getComponent<RigidbodyComponent>();
		physx::PxTransform relativePose(PhysicsVec3fromglmVec3(bCollide.offset));
		physx::PxMaterial* aMaterial = physics->getPhysics()->createMaterial(bCollide.staticFriction, bCollide.dynamicFriction, bCollide.restitution);
		physx::PxShape* shape = physx::PxRigidActorExt::createExclusiveShape(*actor, physx::PxSphereGeometry(tc.GetWorldScale().x * bCollide.radius), *aMaterial);
		shape->setLocalPose(relativePose);
		bCollide.shape = (void*)shape;
		bCollide.rigidbody = (void*)actor;
		rb.body = actor;
		physics->getScene()->addActor(*actor);
	}
	void MeshColliderComponent::create() {
		auto physics = gameObject.getScene()->getPhysics();

		if (!gameObject.getScene()->getData().enablePhsyics3D)
			return;
		auto& tc = gameObject.getComponent<TransformComponent>();
		auto& bCollide = gameObject.getComponent<MeshColliderComponent>();

		if (!gameObject.hasComponent<MeshFilterComponent>())
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
			actor = physics->getPhysics()->createRigidStatic(physx::PxTransform(PhysicsVec3fromglmVec3(tc.GetWorldPosition()), PhysicsQuatfromglmQuat(tc.GetWorldRotation())));
			if (gameObject.hasComponent<RigidbodyComponent>()) {
				actor = physics->createActor(gameObject.getComponent<RigidbodyComponent>(), tc.GetWorldPosition(), tc.GetWorldRotation());
			}
			else {
				RigidbodyComponent& rb = gameObject.addComponent<RigidbodyComponent>();
				rb.useGravity = true;
				rb.kinematic = true;
				actor = physics->createActor(gameObject.getComponent<RigidbodyComponent>(), tc.GetWorldPosition(), tc.GetWorldRotation());
			}
		}
		else {
			actor = physics->getPhysics()->createRigidStatic(physx::PxTransform(PhysicsVec3fromglmVec3(tc.GetWorldPosition()), PhysicsQuatfromglmQuat(tc.GetWorldRotation())));
		}
		physx::PxMaterial* aMaterial = physics->getPhysics()->createMaterial(bCollide.staticFriction, bCollide.dynamicFriction, bCollide.restitution);
		physx::PxShape* shape;
		if (!convex) {
			physx::PxTriangleMesh* physxMesh = physics->createTriangleMesh(bCollide.mesh.mesh);
			if (physxMesh) {
				shape = physx::PxRigidActorExt::createExclusiveShape(*actor, physx::PxTriangleMeshGeometry(physxMesh,
					physx::PxMeshScale(PhysicsVec3fromglmVec3(tc.GetWorldScale()))), *aMaterial);
			}
			else {
				return;
			}
		}
		else {
			physx::PxConvexMesh* physxMesh = physics->createConvexMesh(bCollide.convexMesh);
			if (physxMesh) {
				shape = physx::PxRigidActorExt::createExclusiveShape(*actor, physx::PxConvexMeshGeometry(physxMesh,
					physx::PxMeshScale(PhysicsVec3fromglmVec3(tc.GetWorldScale()))), *aMaterial);
			}
			else {
				return;
			}

		}
		physics->getScene()->addActor(*actor);
		bCollide.shape = (void*)shape;
		bCollide.rigidbody = (void*)actor;
		if(gameObject.hasComponent<RigidbodyComponent>())
			gameObject.getComponent<RigidbodyComponent>().body = actor;
	}
	void CapsuleColliderComponent::create(){
		auto physics = gameObject.getScene()->getPhysics();

		auto& tc = gameObject.getComponent<TransformComponent>();
		auto& bCollide = gameObject.getComponent<CapsuleColliderComponent>();

		physx::PxRigidActor* actor;
		if (gameObject.hasComponent<RigidbodyComponent>()) {
			RigidbodyComponent& rb = gameObject.getComponent<RigidbodyComponent>();
			if (rb.body)
				actor = (physx::PxRigidActor*)rb.body;
			else
				actor = physics->createActor(rb, tc.GetWorldPosition(), tc.GetWorldRotation());
		}
		else {
			RigidbodyComponent& rb = gameObject.addComponent<RigidbodyComponent>();
			rb.useGravity = true;
			rb.kinematic = true;
			actor = physics->createActor(rb, tc.GetWorldPosition(), tc.GetWorldRotation());
		}
		RigidbodyComponent& rb = gameObject.getComponent<RigidbodyComponent>();
		physx::PxTransform relativePose(PhysicsVec3fromglmVec3(bCollide.offset));
		physx::PxMaterial* aMaterial = physics->getPhysics()->createMaterial(bCollide.staticFriction, bCollide.dynamicFriction, bCollide.restitution);

		physx::PxCapsuleGeometry geometry;
		if(horizontal)
			geometry = physx::PxCapsuleGeometry(tc.GetWorldScale().x * bCollide.radius/2.0f, tc.GetWorldScale().y * (height / 4.0f));
		else
			geometry = physx::PxCapsuleGeometry(tc.GetWorldScale().y * bCollide.radius/2.0f, tc.GetWorldScale().x * (height / 4.0f));

		physx::PxShape* shape = physx::PxRigidActorExt::createExclusiveShape(*actor, geometry, *aMaterial);
		shape->setLocalPose(relativePose);
		bCollide.shape = (void*)shape;
		bCollide.rigidbody = (void*)actor;
		rb.body = actor;
		physics->getScene()->addActor(*actor);
	}

}