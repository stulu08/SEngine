#include "st_pch.h"
#include "Transform.h"

#include "Stulu/Physics/Components/RigidbodyComponent.h"

namespace Stulu {
	void TransformComponent::SyncWithPhysics(bool onlyPosition) {
		if (gameObject.getScene()->PhysicsEnable() && gameObject.hasComponent<RigidActorComponent>()) {
			if (onlyPosition)
				gameObject.getComponent<RigidActorComponent>().SetPosition(GetWorldPosition());
			else
				gameObject.getComponent<RigidActorComponent>().SetTransform(GetWorldPosition(), GetWorldRotation());
		}
			

		for (entt::entity child : GetChildren()) {
			GetScene()->getRegistry().get<TransformComponent>(child).SyncWithPhysics(onlyPosition);
		}
	}
}