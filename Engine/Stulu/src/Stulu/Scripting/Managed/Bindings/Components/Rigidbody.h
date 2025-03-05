#pragma once

namespace StuluBindings {
	class Rigidbody {
	public:
		static inline void addForce(uint64_t go, struct Vector3* force, uint32_t mode) {
			Stulu::GameObject((entt::entity)go, GetCurrentScene()).getComponent<Stulu::RigidbodyComponent>().addForce (force->toNative(), (Stulu::RigidbodyComponent::ForceMode)mode);
		}
		static inline void getMassCenterPos(uint64_t go, struct Vector3* pos) {
			glm::vec3& npos = Stulu::GameObject((entt::entity)go, GetCurrentScene()).getComponent<Stulu::RigidbodyComponent>().massCenterPos;
			pos->x = npos.x;
			pos->y = npos.y;
			pos->z = npos.z;
		}
		static inline void setMassCenterPos(uint64_t go, struct Vector3* pos) {
			auto& rb = Stulu::GameObject((entt::entity)go, GetCurrentScene()).getComponent<Stulu::RigidbodyComponent>();
			rb.massCenterPos = pos->toNative();
			rb.updateFlags();
			
		}
		static inline void setRetainAccel(uint64_t go, bool val) {
			auto& rb = Stulu::GameObject((entt::entity)go, GetCurrentScene()).getComponent<Stulu::RigidbodyComponent>();
			rb.retainAccelaration = val;
			rb.updateFlags();
		}
		static inline bool getRetainAccel(uint64_t go) {
			return Stulu::GameObject((entt::entity)go, GetCurrentScene()).getComponent<Stulu::RigidbodyComponent>().retainAccelaration;
		}
		static inline void setKinematic(uint64_t go, bool val) {
			auto& rb = Stulu::GameObject((entt::entity)go, GetCurrentScene()).getComponent<Stulu::RigidbodyComponent>();
			rb.kinematic = val;
			rb.updateFlags();
		}
		static inline bool getKinematic(uint64_t go) {
			return Stulu::GameObject((entt::entity)go, GetCurrentScene()).getComponent<Stulu::RigidbodyComponent>().kinematic;
		}
		static inline void setuseGravity(uint64_t go, bool val) {
			auto& rb = Stulu::GameObject((entt::entity)go, GetCurrentScene()).getComponent<Stulu::RigidbodyComponent>();
			rb.useGravity = val;
			rb.updateFlags();
		}
		static inline bool getuseGravity(uint64_t go) {
			return Stulu::GameObject((entt::entity)go, GetCurrentScene()).getComponent<Stulu::RigidbodyComponent>().useGravity;
		}
		static inline void setMass(uint64_t go, float val) {
			auto& rb = Stulu::GameObject((entt::entity)go, GetCurrentScene()).getComponent<Stulu::RigidbodyComponent>();
			rb.mass = val;
			rb.updateFlags();
		}
		static inline float getMass(uint64_t go) {
			return Stulu::GameObject((entt::entity)go, GetCurrentScene()).getComponent<Stulu::RigidbodyComponent>().mass;
		}
	};
}