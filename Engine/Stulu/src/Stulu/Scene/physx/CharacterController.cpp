#include "st_pch.h"
#include "CharacterController.h"

#include "Collider.h"
#include "Stulu/Scene/Components/Components.h"
#include "PxPhysicsAPI.h"

namespace Stulu {
    void CharacterController::updatePosition(const glm::vec3& pos, const glm::vec3& up) {
        if (!controller)
            return;
        physx::PxController* controller = (physx::PxController*)this->controller;
        controller->setPosition(physx::PxExtendedVec3(pos.x, pos.y, pos.z));
        controller->setUpDirection(PhysicsVec3fromglmVec3(up));
    }
	void CharacterController::update(const glm::vec3& pos, const glm::vec3& up) {
        if (!controller)
            return;
        physx::PxController* con = (physx::PxController*)this->controller;
        physx::PxCapsuleController* controller = (physx::PxCapsuleController*)con;

        controller->setPosition(physx::PxExtendedVec3(pos.x, pos.y, pos.z));
        controller->setUpDirection(PhysicsVec3fromglmVec3(up));

        controller->setHeight(height);
        controller->setRadius(radius);
        controller->resize(height);//I think i should keep this so the controller knows we are updating it
        controller->setSlopeLimit(glm::cos(glm::radians(slopeOffset)));
        controller->setContactOffset(skinWidth);
        controller->setStepOffset(stepOffset);

        

        controller->setNonWalkableMode((physx::PxControllerNonWalkableMode::Enum)((int)nonWalkAbleMode));
        controller->setClimbingMode((physx::PxCapsuleClimbingMode::Enum)((int)climbingMode));
	}
    void CharacterController::destroy() {

    }
}
