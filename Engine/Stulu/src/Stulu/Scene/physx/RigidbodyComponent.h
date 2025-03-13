#pragma once
#include "Stulu/Math/Math.h"
#include "Stulu/Scene/GameObject.h"
#include "Stulu/Scene/Components/Component.h"
#include "PhysX.h"

namespace Stulu {
	class STULU_API RigidbodyComponent : public Component {
	public:
		enum class ForceMode {
			Force,
			Impulse,
			VelocityChange,
			Acceleration
		};

		RigidbodyComponent() = default;
		RigidbodyComponent(const RigidbodyComponent&) = default;

		void addForce(glm::vec3 force, ForceMode mode);
		void setPosition(glm::vec3 position);
		void setTransform(glm::vec3 position, glm::quat rotation);

		void syncTransform();
		void syncTransform(const std::vector<entt::entity>& childs);

		void* getRuntimeBody() { return body; }

		void updateFlags();

		bool useGravity = true;
		//dynamic
		bool rotationX = true, rotationY = true, rotationZ = true;
		bool moveX = true, moveY = true, moveZ = true;
		bool kinematic = false;
		bool retainAccelaration = false;
		float mass = 1.0f;
		glm::vec3 massCenterPos = glm::vec3(.0f);
	private:
		//runtime
		void* body = nullptr;
		RigidbodyComponent(void* body);
		void destroy() override;


		friend class Scene;
		friend class PhysX;
		friend class Collider;
		friend class BoxColliderComponent;
		friend class SphereColliderComponent;
		friend class CapsuleColliderComponent;
		friend class MeshColliderComponent;
	};
}