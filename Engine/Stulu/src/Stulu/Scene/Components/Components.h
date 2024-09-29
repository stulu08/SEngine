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

namespace Stulu {

	class GameObjectBaseComponent : public Component {
	private:
		UUID uuid;
	public:
		inline UUID getUUID() const {
			return uuid;
		}
		inline void updateUUID(const UUID& newUuid) {
			auto& map = gameObject.getScene()->m_uuidGameObjectMap;
			if(map.find(uuid) != map.end())
				map.erase(uuid);

			this->uuid = newUuid;
			map[newUuid] = gameObject;
		}

		std::string name = "GameObject";
		std::string tag = "default";

		GameObjectBaseComponent() = default;
		GameObjectBaseComponent(const GameObjectBaseComponent&) = default;
		GameObjectBaseComponent(const std::string& name, const UUID& uuid)
			: name(name), uuid(uuid) {};
	};
	class TransformComponent : public Component {
	public:
		glm::vec3 position = glm::vec3(.0f);
		glm::quat rotation = glm::quat(1.0f, .0f, .0f, .0f);
		glm::vec3 scale = glm::vec3(1.0f);

		//used for rendering, do not change
		glm::vec3 worldPosition = glm::vec3(0.0f);
		glm::quat worldRotation = glm::quat(1.0f, .0f, .0f, .0f);
		glm::vec3 worldScale = glm::vec3(0.0f);
		glm::vec3 eulerAnglesDegrees = glm::vec3(.0f);//in degrees
		glm::vec3 eulerAnglesWorldDegrees = glm::vec3(.0f);//in degrees

		glm::mat4 transform = glm::mat4(1.0f);
		
		glm::vec3 up = TRANSFORM_UP_DIRECTION;
		glm::vec3 right = TRANSFORM_RIGHT_DIRECTION;
		glm::vec3 forward = TRANSFORM_FOREWARD_DIRECTION;

		//Parent GameObject
		GameObject parent = GameObject::null;

		TransformComponent() = default;
		TransformComponent(const TransformComponent&) = default;
		TransformComponent(const glm::vec3 pos, const glm::quat rotation, const glm::vec3 scale)
			: position(pos), rotation(rotation), scale(scale) { };

		operator const glm::mat4() { return transform; }

		void addChild(GameObject child) { 
			child.getComponent<TransformComponent>().parent = gameObject;
		}
		void addChild(TransformComponent& child) {
			child.parent = gameObject;
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
		ScriptingComponent(const ScriptingComponent& origin) {
			for (auto& script : origin.runtimeScripts) {
				runtimeScripts.push_back(createRef<MonoObjectInstance>(*script));
			}
		}
		std::vector<Ref<MonoObjectInstance>> runtimeScripts;
	};


	
}