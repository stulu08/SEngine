#pragma once
#include "Stulu/Core/UUID.h"
#include "Stulu/Core/Timestep.h"

#include "Stulu/Math/Math.h"

#include "Stulu/Renderer/Renderer2D.h"

#include "Stulu/Scene/AssetsManager.h"
#include "Stulu/Scene/Material.h"

#include "Stulu/Scene/Components/Component.h"
#include "Stulu/Scripting/Managed/MonoObjectInstance.h"
#include "Stulu/Scene/Components/Camera.h"
#include "Stulu/Scene/physx/Collider.h"

#include "Stulu/Scripting/Managed/AssemblyManager.h"

namespace Stulu {

	class GameObjectBaseComponent : public Component {
	public:
		std::string name = "GameObject";
		std::string tag = "Default";

		GameObjectBaseComponent() = default;
		GameObjectBaseComponent(const GameObjectBaseComponent&) = default;
		GameObjectBaseComponent(const std::string& name)
			: name(name) {};
	};
	// defined here to keep the function inline and prevent Declaraton problems
	inline bool GameObject::IsValid() const {
		if (m_entity == entt::null || m_scene == nullptr)
			return false;
		return hasComponent<GameObjectBaseComponent>();
	}

	class TransformComponent : public Component {
	public:
		glm::vec3 position = glm::vec3(0.0f);
		glm::quat rotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
		glm::vec3 scale = glm::vec3(1.0f);

	private:
		mutable glm::mat4 transform = glm::mat4(1.0f);
		mutable glm::vec3 worldPosition = glm::vec3(0.0f);
		mutable glm::quat worldRotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
		mutable glm::vec3 worldScale = glm::vec3(1.0f);
		mutable bool dirty;

		//mutable bool dirty : 1;
		//mutable bool isStatic : 1;

		entt::entity parentEntity = entt::null;
		std::vector<entt::entity> children;

		inline Scene* GetScene() const {
			return gameObject.getScene();
		}
		void MarkDirty() {
			dirty = true;
			auto& registry = GetScene()->getRegistry();
			for (auto& child : children) {
				registry.get<TransformComponent>(child).MarkDirty();
			}
		}
	public:
		TransformComponent()
			: dirty(true) {}
		TransformComponent(const TransformComponent& other) 
			: Component(other) {
			this->position = other.position;
			this->rotation = other.rotation;
			this->scale = other.scale;
			this->parentEntity = other.parentEntity;
			this->children = other.children;
			this->dirty = true;
		}

		TransformComponent(const glm::vec3& pos, const glm::quat& rot, const glm::vec3& scl)
			: position(pos), rotation(rot), scale(scl), dirty(true) {}

		inline void SetPosition(const glm::vec3& pos) { position = pos; MarkDirty(); }
		inline void SetRotation(const glm::quat& rot) { rotation = rot; MarkDirty(); }
		inline void SetScale(const glm::vec3& scl) { scale = scl; MarkDirty(); }

		inline GameObject GetParent() const {
			if (parentEntity == entt::null)
				return GameObject::null;

			return { parentEntity, GetScene() };
		}
		inline bool HasParent() const {
			return GetParent().IsValid();
		}
		inline void SetParent(const GameObject& newParent) {
			GameObject parent = GetParent();

			if (parent.IsValid()) {
				parent.getComponent<TransformComponent>().RemoveChild(gameObject);
			}
			if (newParent.IsValid()) {
				newParent.getComponent<TransformComponent>().AddChild(gameObject);
			}
		}
		inline void AddChild(const GameObject& child) {
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
		inline void RemoveChild(const GameObject& child) {
			children.erase(std::remove(children.begin(), children.end(), child.GetID()), children.end());

			auto& childTransform = child.getComponent<TransformComponent>();
			childTransform.parentEntity = entt::null;
			childTransform.MarkDirty();
		}

		inline std::vector<entt::entity>& GetChildren() { return children; }
		inline const std::vector<entt::entity>& GetChildren() const { return children; }
		inline bool HasChildren() const { return !children.empty(); }

		inline const glm::mat4& GetWorldTransform() const {
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
		// In degrees
		inline glm::vec3 GetWorldEulerRotation() const {
			glm::vec3 eulerAngles = glm::degrees(glm::eulerAngles(worldRotation));
			// Normalize angles to 0 - 360 degrees
			if (eulerAngles.x < 0) eulerAngles.x += 360.0f;
			if (eulerAngles.y < 0) eulerAngles.y += 360.0f;
			if (eulerAngles.z < 0) eulerAngles.z += 360.0f;
			return eulerAngles;
		}
		// In degrees
		inline glm::vec3 GetEulerRotation() const {
			glm::vec3 eulerAngles = glm::degrees(glm::eulerAngles(rotation));
			// Normalize angles to 0 - 360 degrees
			if (eulerAngles.x < 0) eulerAngles.x += 360.0f;
			if (eulerAngles.y < 0) eulerAngles.y += 360.0f;
			if (eulerAngles.z < 0) eulerAngles.z += 360.0f;
			return eulerAngles;
		}

		inline void SetWorldPosition(const glm::vec3& newWorldPos) {
			GameObject parent = GetParent();
			if (parent.IsValid()) {
				auto& parentTransform = parent.getComponent<TransformComponent>();
				position = newWorldPos - parentTransform.GetWorldPosition();
			}
			else {
				position = newWorldPos;
			}
			MarkDirty();
		}
		inline void SetWorldRotation(const glm::quat& newWorldRot) {
			GameObject parent = GetParent();
			if (parent.IsValid()) {
				auto& parentTransform = parent.getComponent<TransformComponent>();
				rotation = glm::inverse(parentTransform.GetWorldRotation()) * newWorldRot;
			}
			else {
				rotation = newWorldRot;
			}
			MarkDirty();
		}
		inline void SetWorldScale(const glm::vec3& newWorldScale) {
			GameObject parent = GetParent();
			if (parent.IsValid()) {
				auto& parentTransform = parent.getComponent<TransformComponent>();
				scale = newWorldScale / parentTransform.GetWorldScale();
			}
			else {
				scale = newWorldScale;
			}
			MarkDirty();
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
};


	class SpriteRendererComponent : public Component {
	public:
		glm::vec4 color = COLOR_WHITE;
		glm::vec2 tiling = glm::vec2(1.0f);
		
		Ref<Texture> texture = nullptr;

		SpriteRendererComponent() = default;
		SpriteRendererComponent(const SpriteRendererComponent&) = default;
		SpriteRendererComponent(const glm::vec4 color)
			: color(color) {};
	};
	class CircleRendererComponent : public Component {
	public:
		glm::vec4 color = COLOR_WHITE;
		float thickness = 1.0f;
		float fade = .005f;

		CircleRendererComponent() = default;
		CircleRendererComponent(const CircleRendererComponent&) = default;
		CircleRendererComponent(const glm::vec4 color)
			: color(color) {};
	};

	class LightComponent : public Component {
	public:
		enum Type{ Directional = 0, Area = 1, Spot = 2 };

		Type lightType = Type::Directional;
		glm::vec3 color = glm::vec3(1.0f);
		float strength = 1.0f;

		float areaRadius = 1.0f;

		float spotLight_cutOff = 10.0f;
		float spotLight_outerCutOff = 15.0f;

		LightComponent() = default;
		LightComponent(const LightComponent&) = default;
		LightComponent(const Type& type)
			: lightType(type) {};
	};
	class SkyBoxComponent : public Component {
	public:
		Ref<SkyBox> texture = nullptr;
		glm::vec3 rotation = { 0,0,0 };
		enum class MapType {
			EnvironmentMap =0, IrradianceMap=1, PrefilterMap=2
		}mapType = MapType::EnvironmentMap;

		SkyBoxComponent() = default;
		SkyBoxComponent(const SkyBoxComponent&) = default;
	};

	class MeshRendererComponent : public Component {
	public:
		Ref<Material> material = nullptr;
		CullMode cullmode = CullMode::Back;

		MeshRendererComponent() = default;
		MeshRendererComponent(const MeshRendererComponent&) = default;

		virtual void onComponentAdded(Scene* scene) override;
	};
	class MeshFilterComponent : public Component {
	public:
		MeshAsset mesh{"",nullptr};

		MeshFilterComponent() = default;
		MeshFilterComponent(const MeshFilterComponent&) = default;
		MeshFilterComponent(const Ref<Mesh>& _mesh) { setMesh(_mesh); }
		MeshFilterComponent(MeshAsset& mesh)
			: mesh(mesh){}

		void setMesh(Ref<Mesh> meshObj, const std::string& name = "New Mesh") {
			if (meshObj)
				mesh.hasMesh = true;
			mesh.mesh = meshObj;
			mesh.name = name;
		}
		void setMesh(MeshAsset meshAsset) {
			mesh = meshAsset;
		}

		virtual void onComponentAdded(Scene* scene) override {
			gameObject.saveAddComponent<MeshRendererComponent>();
		}
	};
	inline void MeshRendererComponent::onComponentAdded(Scene* scene) {
		gameObject.saveAddComponent<MeshFilterComponent>();
	}
	class PostProcessingComponent : public Component {
	public:
		PostProcessingComponent() {
			data = PostProcessingData();
		}
		PostProcessingComponent(const PostProcessingComponent&) = default;

		PostProcessingData data;
	};
	class STULU_API MonoObjectInstance;
	class ScriptingComponent : public Component {
	public:
		ScriptingComponent() = default;
		ScriptingComponent(const ScriptingComponent& other) 
			: Component(other) {
			for (auto& script : other.runtimeScripts) {
				runtimeScripts.push_back(createRef<MonoObjectInstance>(*script));
			}
		}
		std::vector<Ref<MonoObjectInstance>> runtimeScripts;

		Mono::Array FetchObjectArray() {
			const auto& manager = Application::get().getAssemblyManager();

			Mono::Array list = Mono::Array::New(manager->getCoreDomain(), manager->getComponentClass(), runtimeScripts.size());
			for (size_t i = 0; i < runtimeScripts.size(); i++) {
				list.SetRef(i, runtimeScripts[i]->getObject());
			}
			return list;
		}
		
	};


	
}