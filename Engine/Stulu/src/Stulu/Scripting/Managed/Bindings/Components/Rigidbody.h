#pragma once

namespace StuluBindings {
	class Rigidbody {
	public:
		static inline void addForce(uint64_t go, struct Vector3* force, uint32_t mode) {
			Stulu::GameObject((entt::entity)go, GetCurrentScene()).getComponent<Stulu::RigidbodyComponent>().AddForce(force->toNative(), (Stulu::ForceMode)mode);
		}
		static inline void getMassCenterPos(uint64_t go, struct Vector3* pos) {
			glm::vec3& npos = Stulu::GameObject((entt::entity)go, GetCurrentScene()).getComponent<Stulu::RigidbodyComponent>().GetMassCenterPosition();
			pos->x = npos.x;
			pos->y = npos.y;
			pos->z = npos.z;
		}
		static inline void setMassCenterPos(uint64_t go, struct Vector3* pos) {
			auto& rb = Stulu::GameObject((entt::entity)go, GetCurrentScene()).getComponent<Stulu::RigidbodyComponent>();
			rb.SetMassCenterPosition(pos->toNative());
			
		}
		static inline void setRetainAccel(uint64_t go, bool val) {
			auto& rb = Stulu::GameObject((entt::entity)go, GetCurrentScene()).getComponent<Stulu::RigidbodyComponent>();
			rb.SetRetainAcceleration(val);
		}
		static inline bool getRetainAccel(uint64_t go) {
			return Stulu::GameObject((entt::entity)go, GetCurrentScene()).getComponent<Stulu::RigidbodyComponent>().IsRetainingAcceleration();
		}
		static inline void setKinematic(uint64_t go, bool val) {
			auto& rb = Stulu::GameObject((entt::entity)go, GetCurrentScene()).getComponent<Stulu::RigidbodyComponent>();
			rb.SetKinematic(val);
		}
		static inline bool getKinematic(uint64_t go) {
			return Stulu::GameObject((entt::entity)go, GetCurrentScene()).getComponent<Stulu::RigidbodyComponent>().IsKinematic();
		}
		static inline void setuseGravity(uint64_t go, bool val) {
			auto& rb = Stulu::GameObject((entt::entity)go, GetCurrentScene()).getComponent<Stulu::RigidbodyComponent>();
			rb.SetUseGravity(val);
		}
		static inline bool getuseGravity(uint64_t go) {
			return Stulu::GameObject((entt::entity)go, GetCurrentScene()).getComponent<Stulu::RigidbodyComponent>().IsUsingGravity();
		}
		static inline void setMass(uint64_t go, float val) {
			auto& rb = Stulu::GameObject((entt::entity)go, GetCurrentScene()).getComponent<Stulu::RigidbodyComponent>();
			rb.SetMass(val);
		}
		static inline float getMass(uint64_t go) {
			return Stulu::GameObject((entt::entity)go, GetCurrentScene()).getComponent<Stulu::RigidbodyComponent>().GetMass();
		}
	};
}