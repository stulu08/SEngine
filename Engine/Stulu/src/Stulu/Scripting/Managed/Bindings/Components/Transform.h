#pragma once
#include <Stulu/Scene/physx/RigidbodyComponent.h>

namespace StuluBindings {
	class Transform {
	public:
		static inline void getPos(uint64_t go, struct Vector3* pos) {
			glm::vec3& npos = Stulu::GameObject((entt::entity)go, GetCurrentScene()).getComponent<Stulu::TransformComponent>().position;
			pos->x = npos.x;
			pos->y = npos.y;
			pos->z = npos.z;
		}
		static inline void getWorldPos(uint64_t go, struct Vector3* pos) {
			glm::vec3& npos = Stulu::GameObject((entt::entity)go, GetCurrentScene()).getComponent<Stulu::TransformComponent>().GetWorldPosition();
			pos->x = npos.x;
			pos->y = npos.y;
			pos->z = npos.z;
		}
		static inline void setPos(uint64_t go, struct Vector3* pos) {
			Stulu::GameObject object = Stulu::GameObject((entt::entity)go, GetCurrentScene());
			object.getComponent<Stulu::TransformComponent>().SetPosition(pos->toNative());

			if (GetCurrentScene()->getData().enablePhsyics3D && object.hasComponent<Stulu::RigidbodyComponent>()) {
				object.getComponent<Stulu::RigidbodyComponent>().syncTransform();
			}
		}

		static inline void getRotation(uint64_t go, struct Quaternion* rot) {
			glm::quat& nrot = Stulu::GameObject((entt::entity)go, GetCurrentScene()).getComponent<Stulu::TransformComponent>().rotation;
			rot->x = nrot.x;
			rot->y = nrot.y;
			rot->z = nrot.z;
			rot->w = nrot.w;
		}
		static inline void getRotationEulerDegree(uint64_t go, struct Vector3* rot) {
			glm::vec3& nrot = Stulu::GameObject((entt::entity)go, GetCurrentScene()).getComponent<Stulu::TransformComponent>().GetEulerRotation();
			rot->x = nrot.x;
			rot->y = nrot.y;
			rot->z = nrot.z;
		}
		static inline void getWorldRotation(uint64_t go, struct Quaternion* rot) {
			glm::quat& nrot = Stulu::GameObject((entt::entity)go, GetCurrentScene()).getComponent<Stulu::TransformComponent>().GetWorldRotation();
			rot->x = nrot.x;
			rot->y = nrot.y;
			rot->z = nrot.z;
			rot->w = nrot.w;
		}
		static inline void setRotation(uint64_t go, struct Quaternion* rot) {
			Stulu::GameObject object = Stulu::GameObject((entt::entity)go, GetCurrentScene());
			object.getComponent<Stulu::TransformComponent>().SetRotation(rot->toNative());

			if (GetCurrentScene()->getData().enablePhsyics3D && object.hasComponent<Stulu::RigidbodyComponent>()) {
				object.getComponent<Stulu::RigidbodyComponent>().syncTransform();
			}
		}

		static inline void getScale(uint64_t go, struct Vector3* scale) {
			glm::vec3& nscale = Stulu::GameObject((entt::entity)go, GetCurrentScene()).getComponent<Stulu::TransformComponent>().scale;
			scale->x = nscale.x;
			scale->y = nscale.y;
			scale->z = nscale.z;
		}
		static inline void getWorldScale(uint64_t go, struct Vector3* scale) {
			glm::vec3& nscale = Stulu::GameObject((entt::entity)go, GetCurrentScene()).getComponent<Stulu::TransformComponent>().GetWorldScale();
			scale->x = nscale.x;
			scale->y = nscale.y;
			scale->z = nscale.z;
		}
		static inline void setScale(uint64_t go, struct Vector3* scale) {
			Stulu::GameObject((entt::entity)go, GetCurrentScene()).getComponent<Stulu::TransformComponent>().SetScale(scale->toNative());
		}
		static inline void up(uint64_t go, struct Vector3* up) {
			glm::vec3& val = Stulu::GameObject((entt::entity)go, GetCurrentScene()).getComponent<Stulu::TransformComponent>().GetUp();
			up->x = val.x;
			up->y = val.y;
			up->z = val.z;
		}
		static inline void forward(uint64_t go, struct Vector3* forward) {
			glm::vec3& val = Stulu::GameObject((entt::entity)go, GetCurrentScene()).getComponent<Stulu::TransformComponent>().GetForward();
			forward->x = val.x;
			forward->y = val.y;
			forward->z = val.z;
		}
		static inline void right(uint64_t go, struct Vector3* right) {
			glm::vec3& val = Stulu::GameObject((entt::entity)go, GetCurrentScene()).getComponent<Stulu::TransformComponent>().GetRight();
			right->x = val.x;
			right->y = val.y;
			right->z = val.z;
		}
	};
}