#include "st_pch.h"
#include "Transform.h"

#include "Stulu/Physics/Components/RigidbodyComponent.h"

namespace Stulu {
	void TransformComponent::SyncWithPhysics(bool onlyPosition) {
		if (gameObject.GetRegistry()->IsScene() && gameObject.hasComponent<RigidActorComponent>()) {
			if (onlyPosition)
				gameObject.getComponent<RigidActorComponent>().SetPosition(GetWorldPosition());
			else
				gameObject.getComponent<RigidActorComponent>().SetTransform(GetWorldPosition(), GetWorldRotation());
		}
			

		for (entt::entity child : GetChildren()) {
			GetRegistry()->GetRegistry().get<TransformComponent>(child).SyncWithPhysics(onlyPosition);
		}
	}
}