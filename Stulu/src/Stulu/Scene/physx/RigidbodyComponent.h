#pragma once
#include "Stulu/Math/Math.h"
namespace Stulu {
	class STULU_API RigidbodyComponent {
	public:
		enum class Type { Dynamic = 0, Static = 1 };
		enum class ForceMode {
			Force,
			Impulse,
			VelocityChange,
			Acceleration
		};

		RigidbodyComponent() = default;
		RigidbodyComponent(const RigidbodyComponent&) = default;

		void addForce(glm::vec3 force, ForceMode mode);
		
		void* getRuntimeBody() { return body; }

		Type rbType = Type::Dynamic;
		bool useGravity = true;
		//dynamic
		bool rotationX = true, rotationY = true, rotationZ = true;
		bool kinematic = false;
		bool retainAccelaration = false;
		float mass = 1.0f;
		glm::vec3 massCenterPos = glm::vec3(.0f);
	private:
		//runtime
		void* body = nullptr;

		friend class Scene;
		friend class PhysX;
	};
}