#pragma once
#include "Stulu/Scripting/Managed/Bindings/Bindings.h"
#include "Stulu/Physics/Components/Collider.h"

namespace StuluBindings {
	struct PhysMaterial {
		float sF, dF, re;
	};

	class BoxCollider {
	public:
		static inline void setMaterial(uint64_t entityID, struct PhysMaterial* mat) {
			Stulu::GameObject go = { (entt::entity)entityID, GetCurrentRegistry() };
			auto& collider = go.getComponent<Stulu::BoxColliderComponent>();
			collider.material.SetStaticFriction(mat->sF);
			collider.material.SetDynamicFriction(mat->dF);
			collider.material.SetRestitution(mat->re);
		}
		static inline void getMaterial(uint64_t entityID, struct PhysMaterial* mat) {
			Stulu::GameObject go = { (entt::entity)entityID, GetCurrentRegistry() };
			auto& collider = go.getComponent<Stulu::BoxColliderComponent>();
			mat->sF = collider.material.StaticFriction;
			mat->dF = collider.material.DynamicFriction;
			mat->re = collider.material.Restitution;
		}

		static void setOffset(uint64_t entityId, struct Vector3* value) {
			Stulu::GameObject go = { (entt::entity)entityId, GetCurrentRegistry() };
			go.getComponent<Stulu::BoxColliderComponent>().SetOffset(value->toNative());
		}
		static void getOffset(uint64_t entityId, struct Vector3* outVal) {
			Stulu::GameObject go = { (entt::entity)entityId, GetCurrentRegistry() };
			auto& collider = go.getComponent<Stulu::BoxColliderComponent>();
			outVal->x = collider.GetOffset().x;
			outVal->y = collider.GetOffset().y;
			outVal->z = collider.GetOffset().z;
		}

		static void setSize(uint64_t entityId, struct Vector3* value) {
			Stulu::GameObject go = { (entt::entity)entityId, GetCurrentRegistry() };
			go.getComponent<Stulu::BoxColliderComponent>().SetSize(value->toNative());
		}
		static void getSize(uint64_t entityId, struct Vector3* outVal) {
			Stulu::GameObject go = { (entt::entity)entityId, GetCurrentRegistry() };
			auto& collider = go.getComponent<Stulu::BoxColliderComponent>();
			outVal->x = collider.GetSize().x;
			outVal->y = collider.GetSize().y;
			outVal->z = collider.GetSize().z;
		}
	};

	class SphereCollider {
	public:
		static inline void setMaterial(uint64_t entityID, struct PhysMaterial* mat) {
			Stulu::GameObject go = { (entt::entity)entityID, GetCurrentRegistry() };
			auto& collider = go.getComponent<Stulu::SphereColliderComponent>();
			collider.material.SetStaticFriction(mat->sF);
			collider.material.SetDynamicFriction(mat->dF);
			collider.material.SetRestitution(mat->re);
		}
		static inline void getMaterial(uint64_t entityID, struct PhysMaterial* mat) {
			Stulu::GameObject go = { (entt::entity)entityID, GetCurrentRegistry() };
			auto& collider = go.getComponent<Stulu::SphereColliderComponent>();
			mat->sF = collider.material.StaticFriction;
			mat->dF = collider.material.DynamicFriction;
			mat->re = collider.material.Restitution;
		}

		static void setOffset(uint64_t entityId, struct Vector3* value) {
			Stulu::GameObject go = { (entt::entity)entityId, GetCurrentRegistry() };
			go.getComponent<Stulu::SphereColliderComponent>().SetOffset(value->toNative());
		}
		static void getOffset(uint64_t entityId, struct Vector3* outVal) {
			Stulu::GameObject go = { (entt::entity)entityId, GetCurrentRegistry() };
			auto& collider = go.getComponent<Stulu::SphereColliderComponent>();
			outVal->x = collider.GetOffset().x;
			outVal->y = collider.GetOffset().y;
			outVal->z = collider.GetOffset().z;
		}

		static void setRadius(uint64_t entityId, float value) {
			Stulu::GameObject go = { (entt::entity)entityId, GetCurrentRegistry() };
			go.getComponent<Stulu::SphereColliderComponent>().SetRadius(value);
		}
		static float getRadius(uint64_t entityId) {
			Stulu::GameObject go = { (entt::entity)entityId, GetCurrentRegistry() };
			return go.getComponent<Stulu::SphereColliderComponent>().GetRadius();
		}
	};

	class CapsuleCollider {
	public:
		static inline void setMaterial(uint64_t entityID, struct PhysMaterial* mat) {
			Stulu::GameObject go = { (entt::entity)entityID, GetCurrentRegistry() };
			auto& collider = go.getComponent<Stulu::CapsuleColliderComponent>();
			collider.material.SetStaticFriction(mat->sF);
			collider.material.SetDynamicFriction(mat->dF);
			collider.material.SetRestitution(mat->re);
		}
		static inline void getMaterial(uint64_t entityID, struct PhysMaterial* mat) {
			Stulu::GameObject go = { (entt::entity)entityID, GetCurrentRegistry() };
			auto& collider = go.getComponent<Stulu::CapsuleColliderComponent>();
			mat->sF = collider.material.StaticFriction;
			mat->dF = collider.material.DynamicFriction;
			mat->re = collider.material.Restitution;
		}

		static void setOffset(uint64_t entityId, struct Vector3* value) {
			Stulu::GameObject go = { (entt::entity)entityId, GetCurrentRegistry() };
			go.getComponent<Stulu::CapsuleColliderComponent>().SetOffset(value->toNative());
		}
		static void getOffset(uint64_t entityId, struct Vector3* outVal) {
			Stulu::GameObject go = { (entt::entity)entityId, GetCurrentRegistry() };
			auto& collider = go.getComponent<Stulu::CapsuleColliderComponent>();
			outVal->x = collider.GetOffset().x;
			outVal->y = collider.GetOffset().y;
			outVal->z = collider.GetOffset().z;
		}

		static void setRadius(uint64_t entityId, float value) {
			Stulu::GameObject go = { (entt::entity)entityId, GetCurrentRegistry() };
			go.getComponent<Stulu::CapsuleColliderComponent>().SetRadius(value);
		}
		static float getRadius(uint64_t entityId) {
			Stulu::GameObject go = { (entt::entity)entityId, GetCurrentRegistry() };
			return go.getComponent<Stulu::CapsuleColliderComponent>().GetRadius();
		}

		static void setHeight(uint64_t entityId, float value) {
			Stulu::GameObject go = { (entt::entity)entityId, GetCurrentRegistry() };
			go.getComponent<Stulu::CapsuleColliderComponent>().SetHeight(value);
		}
		static float getHeight(uint64_t entityId) {
			Stulu::GameObject go = { (entt::entity)entityId, GetCurrentRegistry() };
			return go.getComponent<Stulu::CapsuleColliderComponent>().GetHeight();
		}

		static void setVertical(uint64_t entityId, bool value) {
			Stulu::GameObject go = { (entt::entity)entityId, GetCurrentRegistry() };
			go.getComponent<Stulu::CapsuleColliderComponent>().SetVertical(value);
		}
		static bool getVertical(uint64_t entityId) {
			Stulu::GameObject go = { (entt::entity)entityId, GetCurrentRegistry() };
			return go.getComponent<Stulu::CapsuleColliderComponent>().GetVertical();
		}
	};
}