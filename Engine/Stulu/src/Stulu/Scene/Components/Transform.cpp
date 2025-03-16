#include "st_pch.h"
#include "Transform.h"

#include "Stulu/Scene/physx/RigidbodyComponent.h"

namespace Stulu {
	void TransformComponent::SyncWithPhysics(bool onlyPosition) {
		if (gameObject.hasComponent<RigidbodyComponent>()) {
			if (onlyPosition)
				gameObject.getComponent<RigidbodyComponent>().SetPosition(GetWorldPosition());
			else
				gameObject.getComponent<RigidbodyComponent>().SetTransform(GetWorldPosition(), GetWorldRotation());
		}
		if (gameObject.hasComponent<RigidStaticComponent>()) {
			if (onlyPosition)
				gameObject.getComponent<RigidStaticComponent>().SetPosition(GetWorldPosition());
			else
				gameObject.getComponent<RigidStaticComponent>().SetTransform(GetWorldPosition(), GetWorldRotation());
		}
			

		for (entt::entity child : GetChildren()) {
			GetScene()->getRegistry().get<TransformComponent>(child).SyncWithPhysics(onlyPosition);
		}
	}
}