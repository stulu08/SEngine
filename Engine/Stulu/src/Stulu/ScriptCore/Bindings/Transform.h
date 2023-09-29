#pragma once
#include <Stulu/Scene/Components/Components.h>
#include "Stulu/ScriptCore/Bindings/Types.h"

namespace StuluBindings {
	class Transform {
	public:
		static inline void getPos(uint32_t go, struct Vector3* pos) {
			glm::vec3& npos = Stulu::GameObject((entt::entity)go, Stulu::Scene::activeScene()).getComponent<Stulu::TransformComponent>().position;
			pos->x = npos.x;
			pos->y = npos.y;
			pos->z = npos.z;
		}
		static inline void getWorldPos(uint32_t go, struct Vector3* pos) {
			glm::vec3& npos = Stulu::GameObject((entt::entity)go, Stulu::Scene::activeScene()).getComponent<Stulu::TransformComponent>().worldPosition;
			pos->x = npos.x;
			pos->y = npos.y;
			pos->z = npos.z;
		}
		static inline void setPos(uint32_t go, struct Vector3* pos) {
			Stulu::GameObject object = Stulu::GameObject((entt::entity)go, Stulu::Scene::activeScene());
			object.getComponent<Stulu::TransformComponent>().position = pos->toNative();
			if (Stulu::Scene::activeScene()->getData().enablePhsyics3D) {
				Stulu::Scene::activeScene()->updateTransformAndChangePhysicsPositionAndDoTheSameWithAllChilds(object);
			}
		}

		static inline void getRotation(uint32_t go, struct Quaternion* rot) {
			glm::quat& nrot = Stulu::GameObject((entt::entity)go, Stulu::Scene::activeScene()).getComponent<Stulu::TransformComponent>().rotation;
			rot->x = nrot.x;
			rot->y = nrot.y;
			rot->z = nrot.z;
			rot->w = nrot.w;
		}
		static inline void getRotationInDegree(uint32_t go, struct Vector3* rot) {
			glm::vec3& nrot = glm::radians(Stulu::GameObject((entt::entity)go, Stulu::Scene::activeScene()).getComponent<Stulu::TransformComponent>().eulerAnglesDegrees);
			rot->x = nrot.x;
			rot->y = nrot.y;
			rot->z = nrot.z;
		}
		static inline void getWorldRotation(uint32_t go, struct Quaternion* rot) {
			glm::quat& nrot = Stulu::GameObject((entt::entity)go, Stulu::Scene::activeScene()).getComponent<Stulu::TransformComponent>().worldRotation;
			rot->x = nrot.x;
			rot->y = nrot.y;
			rot->z = nrot.z;
			rot->w = nrot.w;
		}
		static inline void setRotation(uint32_t go, struct Quaternion* rot) {
			Stulu::GameObject object = Stulu::GameObject((entt::entity)go, Stulu::Scene::activeScene());
			object.getComponent<Stulu::TransformComponent>().rotation = rot->toNative();
			if (Stulu::Scene::activeScene()->getData().enablePhsyics3D) {
				Stulu::Scene::activeScene()->updateTransformAndChangePhysicsPositionAndDoTheSameWithAllChilds(object);
			}
		}

		static inline void getScale(uint32_t go, struct Vector3* scale) {
			glm::vec3& nscale = Stulu::GameObject((entt::entity)go, Stulu::Scene::activeScene()).getComponent<Stulu::TransformComponent>().scale;
			scale->x = nscale.x;
			scale->y = nscale.y;
			scale->z = nscale.z;
		}
		static inline void getWorldScale(uint32_t go, struct Vector3* scale) {
			glm::vec3& nscale = Stulu::GameObject((entt::entity)go, Stulu::Scene::activeScene()).getComponent<Stulu::TransformComponent>().worldScale;
			scale->x = nscale.x;
			scale->y = nscale.y;
			scale->z = nscale.z;
		}
		static inline void setScale(uint32_t go, struct Vector3* scale) {
			Stulu::GameObject((entt::entity)go, Stulu::Scene::activeScene()).getComponent<Stulu::TransformComponent>().scale = scale->toNative();
		}
		static inline void up(uint32_t go, struct Vector3* up) {
			glm::vec3& val = Stulu::GameObject((entt::entity)go, Stulu::Scene::activeScene()).getComponent<Stulu::TransformComponent>().up;
			up->x = val.x;
			up->y = val.y;
			up->z = val.z;
		}
		static inline void forward(uint32_t go, struct Vector3* forward) {
			glm::vec3& val = Stulu::GameObject((entt::entity)go, Stulu::Scene::activeScene()).getComponent<Stulu::TransformComponent>().forward;
			forward->x = val.x;
			forward->y = val.y;
			forward->z = val.z;
		}
		static inline void right(uint32_t go, struct Vector3* right) {
			glm::vec3& val = Stulu::GameObject((entt::entity)go, Stulu::Scene::activeScene()).getComponent<Stulu::TransformComponent>().right;
			right->x = val.x;
			right->y = val.y;
			right->z = val.z;
		}
	};
}