#include "st_pch.h"
#include "Transform.h"

#include "Stulu/Scene/physx/RigidbodyComponent.h"

namespace Stulu {
	void TransformComponent::SyncWithPhysics() {
		if (gameObject.hasComponent<RigidbodyComponent>())
			gameObject.getComponent<RigidbodyComponent>().setTransform(GetWorldPosition(), GetWorldRotation());

		for (entt::entity child : GetChildren()) {
			GetScene()->getRegistry().get<TransformComponent>(child).SyncWithPhysics();
		}
	}
}