#pragma once

namespace StuluBindings {
	class Rigidbody {
	public:
		static inline void addForce(uint32_t go, struct Vector3* force, uint32_t mode) {
			Stulu::GameObject((entt::entity)go, Stulu::Scene::activeScene()).getComponent<Stulu::RigidbodyComponent>().addForce (force->toNative(), (Stulu::RigidbodyComponent::ForceMode)mode);
		}
		static inline void getMassCenterPos(uint32_t go, struct Vector3* pos) {
			glm::vec3& npos = Stulu::GameObject((entt::entity)go, Stulu::Scene::activeScene()).getComponent<Stulu::RigidbodyComponent>().massCenterPos;
			pos->x = npos.x;
			pos->y = npos.y;
			pos->z = npos.z;
		}
		static inline void setMassCenterPos(uint32_t go, struct Vector3* pos) {
			auto& rb = Stulu::GameObject((entt::entity)go, Stulu::Scene::activeScene()).getComponent<Stulu::RigidbodyComponent>();
			rb.massCenterPos = pos->toNative();
			rb.updateFlags();
			
		}
		static inline void setRetainAccel(uint32_t go, bool val) {
			auto& rb = Stulu::GameObject((entt::entity)go, Stulu::Scene::activeScene()).getComponent<Stulu::RigidbodyComponent>();
			rb.retainAccelaration = val;
			rb.updateFlags();
		}
		static inline bool getRetainAccel(uint32_t go) {
			return Stulu::GameObject((entt::entity)go, Stulu::Scene::activeScene()).getComponent<Stulu::RigidbodyComponent>().retainAccelaration;
		}
		static inline void setKinematic(uint32_t go, bool val) {
			auto& rb = Stulu::GameObject((entt::entity)go, Stulu::Scene::activeScene()).getComponent<Stulu::RigidbodyComponent>();
			rb.kinematic = val;
			rb.updateFlags();
		}
		static inline bool getKinematic(uint32_t go) {
			return Stulu::GameObject((entt::entity)go, Stulu::Scene::activeScene()).getComponent<Stulu::RigidbodyComponent>().kinematic;
		}
		static inline void setuseGravity(uint32_t go, bool val) {
			auto& rb = Stulu::GameObject((entt::entity)go, Stulu::Scene::activeScene()).getComponent<Stulu::RigidbodyComponent>();
			rb.useGravity = val;
			rb.updateFlags();
		}
		static inline bool getuseGravity(uint32_t go) {
			return Stulu::GameObject((entt::entity)go, Stulu::Scene::activeScene()).getComponent<Stulu::RigidbodyComponent>().useGravity;
		}
		static inline void setMass(uint32_t go, float val) {
			auto& rb = Stulu::GameObject((entt::entity)go, Stulu::Scene::activeScene()).getComponent<Stulu::RigidbodyComponent>();
			rb.mass = val;
			rb.updateFlags();
		}
		static inline float getMass(uint32_t go) {
			return Stulu::GameObject((entt::entity)go, Stulu::Scene::activeScene()).getComponent<Stulu::RigidbodyComponent>().mass;
		}
	};
}