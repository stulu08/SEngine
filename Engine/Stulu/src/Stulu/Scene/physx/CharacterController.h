#pragma once
#include "Stulu/Math/Math.h"
#include "Stulu/Scene/GameObject.h"
#include "Stulu/Scene/Components/Component.h"

#include "PhysX.h"

namespace Stulu {
	class STULU_API CharacterController : public Component {
	public:
		CharacterController() = default;
		CharacterController(const CharacterController&) = default;

		float height = 1.0f;
		float radius = .5f;
		float slopeOffset = 45.0f;
		float skinWidth = .1f;
		float stepOffset = .3f;
		float density = 10.0f;//kinematic actors density

		float material_staticFriction = ST_DEFAULT_PHYSX_MATERIAL_STATIC_FRICTION;
		float material_dynamicFriction = ST_DEFAULT_PHYSX_MATERIAL_DYNAMIC_FRICTION;
		float material_restitution = ST_DEFAULT_PHYSX_MATERIAL_RESTITUTION;

		enum class NonWalkAbleMode {
			PreventClimbing,						//Stops character from climbing up non-walkable slopes, but doesn't move it otherwise
			preventClimbingAndForceSliding		//Stops character from climbing up non-walkable slopes, and forces it to slide down those slopes
		} nonWalkAbleMode = NonWalkAbleMode::PreventClimbing;
		enum class ClimbingMode {
			Easy,			//Standard mode, let the capsule climb over surfaces according to impact normal
			Constrained,	//Constrained mode, try to limit climbing according to the step offset

		}climbingMode = ClimbingMode::Easy;


		void update(const glm::vec3& pos, const glm::vec3& up);
		void updatePosition(const glm::vec3& pos, const glm::vec3& up);
	private:
		void destroy() override;

		void* controller;

		float m_scaleCoeff = .8f;// coef for kinematic actor
		float m_invisibleWallHeight = .0f;//walls created for non visible triangle,if no jumping keep it 0
		float m_maxJumpHeight = .0f;//only if invisibleWallHeight > 0

		friend class Scene;
		friend class PhysX;
	};
}


