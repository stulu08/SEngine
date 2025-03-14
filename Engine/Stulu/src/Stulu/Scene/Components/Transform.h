#pragma once
#include "Component.h"

namespace Stulu {
	class STULU_API TransformComponent : public Component {
	public:
		// read only
		glm::vec3 position = glm::vec3(0.0f);
		glm::quat rotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
		glm::vec3 scale = glm::vec3(1.0f);

		TransformComponent()
			: dirty(true), isStatic(false), updatePhysics(true) {}
		TransformComponent(const TransformComponent& other)
			: Component(other) {
			this->position = other.position;
			this->rotation = other.rotation;
			this->scale = other.scale;
			this->parentEntity = other.parentEntity;
			this->children = other.children;
			this->isStatic = other.isStatic;
			this->updatePhysics = other.updatePhysics;
			this->dirty = true;
		}

		TransformComponent(const glm::vec3& pos, const glm::quat& rot, const glm::vec3& scl)
			: position(pos), rotation(rot), scale(scl), dirty(true) {}

		// manipulate transform
		void SetPosition(const glm::vec3& pos);
		void SetRotation(const glm::quat& rot);
		void SetScale(const glm::vec3& scl);

		void SetWorldPosition(const glm::vec3& newWorldPos);
		void SetWorldRotation(const glm::quat& newWorldRot);
		void SetWorldScale(const glm::vec3& newWorldScale);

		void SetUpdatePhysics(bool value) const { updatePhysics = value; }
		void SetStatic(bool value) const { isStatic = value; }

		// child system
		inline GameObject GetParent() const {
			return { parentEntity, GetScene() };
		}
		inline bool HasParent() const {
			return GetParent().IsValid();
		}
		inline void SetParent(const GameObject& newParent);
		void AddChild(const GameObject& child);
		void RemoveChild(const GameObject& child);

		inline std::vector<entt::entity>& GetChildren() { return children; }
		inline const std::vector<entt::entity>& GetChildren() const { return children; }
		inline bool HasChildren() const { return !children.empty(); }

		// getter
		const glm::mat4& GetWorldTransform() const;
		inline glm::mat4 GetLocalTransform() const {
			return glm::translate(glm::mat4(1.0f), position) *
				glm::mat4_cast(rotation) *
				glm::scale(glm::mat4(1.0f), scale);
		}

		inline glm::vec3 GetWorldPosition() const {
			if (dirty)
				GetWorldTransform();
			return worldPosition;
		}
		inline glm::quat GetWorldRotation() const {
			if (dirty)
				GetWorldTransform();
			return worldRotation;
		}
		inline glm::vec3 GetWorldScale() const {
			if (dirty)
				GetWorldTransform();
			return worldScale;
		}

		inline glm::vec3 GetForward() const {
			return glm::rotate(GetWorldRotation(), TRANSFORM_FOREWARD_DIRECTION);
		}
		inline glm::vec3 GetUp() const {
			return glm::rotate(GetWorldRotation(), TRANSFORM_UP_DIRECTION);
		}
		inline glm::vec3 GetRight() const {
			return glm::rotate(GetWorldRotation(), TRANSFORM_RIGHT_DIRECTION);
		}

		inline bool IsStatic() const { return isStatic; }
		// When static not syncing with physics
		inline bool IsUpdatingPhysics() const { return updatePhysics && !IsStatic(); }

		// In degrees
		inline glm::vec3 GetWorldEulerRotation() const;
		// In degrees
		inline glm::vec3 GetEulerRotation() const;

	private:
		mutable glm::mat4 transform = glm::mat4(1.0f);
		mutable glm::vec3 worldPosition = glm::vec3(0.0f);
		mutable glm::quat worldRotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
		mutable glm::vec3 worldScale = glm::vec3(1.0f);

		mutable bool dirty : 1;
		mutable bool isStatic : 1;
		mutable bool updatePhysics : 1;

		entt::entity parentEntity = entt::null;
		std::vector<entt::entity> children;

		inline Scene* GetScene() const {
			return gameObject.getScene();
		}
		void MarkDirty() {
			if (isStatic) return;

			dirty = true;
			auto& registry = GetScene()->getRegistry();
			for (auto& child : children) {
				registry.get<TransformComponent>(child).MarkDirty();
			}
		}

		void SyncWithPhysics();
	};


	inline const glm::mat4& TransformComponent::GetWorldTransform() const {
		if (isStatic && !dirty) {
			return transform;
		}

		if (dirty) {
			GameObject parent = GetParent();

			if (parent.IsValid()) {
				const glm::mat4 parentTransform = parent.getComponent<TransformComponent>().GetWorldTransform();
				transform = parentTransform * GetLocalTransform();

				Math::decomposeTransform(transform, worldPosition, worldRotation, worldScale);
			}
			else {
				transform = GetLocalTransform();
				worldPosition = position;
				worldRotation = rotation;
				worldScale = scale;
			}
			dirty = false;
		}
		return transform;
	}

	inline void TransformComponent::SetPosition(const glm::vec3& pos) {
		if (isStatic) return;

		position = pos;
		MarkDirty();

		if (IsUpdatingPhysics()) {
			SyncWithPhysics();
		}
	}
	inline void TransformComponent::SetRotation(const glm::quat& rot) {
		if (isStatic) return;

		rotation = rot;
		MarkDirty();

		if (IsUpdatingPhysics()) {
			SyncWithPhysics();
		}
	}
	inline void TransformComponent::SetScale(const glm::vec3& scl) {
		if (isStatic) return;

		scale = scl;
		MarkDirty();
	}

	inline void TransformComponent::SetParent(const GameObject& newParent) {
		GameObject parent = GetParent();
		if (parent.IsValid()) {
			parent.getComponent<TransformComponent>().RemoveChild(gameObject);
		}
		if (newParent.IsValid()) {
			newParent.getComponent<TransformComponent>().AddChild(gameObject);
		}
	}
	inline void TransformComponent::AddChild(const GameObject& child) {
		if (child == gameObject) {
			CORE_WARN("Cannot assign as own parent!");
			return;
		}

		if (std::find(children.begin(), children.end(), child.GetID()) != children.end()) {
			CORE_WARN("Child already present!");
			return;
		}

		children.push_back(child.GetID());
		auto& childTransform = child.getComponent<TransformComponent>();
		childTransform.parentEntity = gameObject.GetID();
		childTransform.MarkDirty();
	}
	inline void TransformComponent::RemoveChild(const GameObject& child) {
		children.erase(std::remove(children.begin(), children.end(), child.GetID()), children.end());

		auto& childTransform = child.getComponent<TransformComponent>();
		childTransform.parentEntity = entt::null;
		childTransform.MarkDirty();
	}

	inline glm::vec3 TransformComponent::GetWorldEulerRotation() const {
		glm::vec3 eulerAngles = glm::degrees(glm::eulerAngles(worldRotation));
		// Normalize angles to 0 - 360 degrees
		if (eulerAngles.x < 0) eulerAngles.x += 360.0f;
		if (eulerAngles.y < 0) eulerAngles.y += 360.0f;
		if (eulerAngles.z < 0) eulerAngles.z += 360.0f;
		return eulerAngles;
	}
	inline glm::vec3 TransformComponent::GetEulerRotation() const {
		glm::vec3 eulerAngles = glm::degrees(glm::eulerAngles(rotation));
		// Normalize angles to 0 - 360 degrees
		if (eulerAngles.x < 0) eulerAngles.x += 360.0f;
		if (eulerAngles.y < 0) eulerAngles.y += 360.0f;
		if (eulerAngles.z < 0) eulerAngles.z += 360.0f;
		return eulerAngles;
	}

	inline void TransformComponent::SetWorldPosition(const glm::vec3& newWorldPos) {
		GameObject parent = GetParent();
		if (parent.IsValid()) {
			auto& parentTransform = parent.getComponent<TransformComponent>();
			SetPosition(newWorldPos - parentTransform.GetWorldPosition());
		}
		else {
			SetPosition(newWorldPos);
		}
	}
	inline void TransformComponent::SetWorldRotation(const glm::quat& newWorldRot) {
		GameObject parent = GetParent();
		if (parent.IsValid()) {
			auto& parentTransform = parent.getComponent<TransformComponent>();
			SetRotation(glm::inverse(parentTransform.GetWorldRotation()) * newWorldRot);
		}
		else {
			SetRotation(newWorldRot);
		}
	}
	inline void TransformComponent::SetWorldScale(const glm::vec3& newWorldScale) {
		GameObject parent = GetParent();
		if (parent.IsValid()) {
			auto& parentTransform = parent.getComponent<TransformComponent>();
			SetScale(newWorldScale / parentTransform.GetWorldScale());
		}
		else {
			SetScale(newWorldScale);
		}
	}
}