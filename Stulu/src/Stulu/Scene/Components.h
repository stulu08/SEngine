#pragma once
#include "Stulu/Core/UUID.h"
#include "Stulu/Core/Timestep.h"

#include "Stulu/Math/Math.h"

#include "Stulu/Renderer/Renderer2D.h"

#include "Stulu/Scene/AssetsManager.h"
#include "Stulu/Scene/Material.h"

#include "Stulu/Scene/Components/Camera.h"

#include "Stulu/Scene/physx/Collider.h"

namespace Stulu {
	struct GameObjectBaseComponent {
		UUID uuid;
		std::string name = "GameObject";
		std::string tag = "default";

		GameObjectBaseComponent() = default;
		GameObjectBaseComponent(const GameObjectBaseComponent&) = default;
		GameObjectBaseComponent(const std::string& name)
			: name(name) { };
	};
	struct TransformComponent {
		glm::vec3 position = glm::vec3(.0f);
		glm::quat rotation = glm::quat(1.0f, .0f, .0f, .0f);
		glm::vec3 scale = glm::vec3(1.0f);

		//used for rendering, do not change
		glm::vec3 worldPosition = glm::vec3(0.0f);
		glm::quat worldRotation = glm::quat(1.0f, .0f, .0f, .0f);
		glm::vec3 worldScale = glm::vec3(0.0f);

		glm::mat4 transform = glm::mat4(1.0f);
		
		glm::vec3 up = TRANSFORM_UP_DIRECTION;
		glm::vec3 right = TRANSFORM_RIGHT_DIRECTION;
		glm::vec3 forward = TRANSFORM_FOREWARD_DIRECTION;

		//this GameObject
		GameObject gameObject = GameObject::null;
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

	struct SpriteRendererComponent {
		glm::vec4 color = COLOR_WHITE;
		glm::vec2 tiling = glm::vec2(1.0f);
		
		Ref<Texture2D> texture = nullptr;

		SpriteRendererComponent() = default;
		SpriteRendererComponent(const SpriteRendererComponent&) = default;
		SpriteRendererComponent(const glm::vec4 color)
			: color(color) {};
	};

	struct LightComponent {
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
	struct SkyBoxComponent {
		Ref<CubeMap> texture = nullptr;

		float blur = .0f;
		enum class MapType {
			EnvironmentMap =0, IrradianceMap=1, PrefilterMap=2
		}mapType = MapType::EnvironmentMap;

		SkyBoxComponent() = default;
		SkyBoxComponent(const SkyBoxComponent&) = default;
	};

	struct MeshRendererComponent {
		Material* material;
		CullMode cullmode = CullMode::Back;

		bool m_enabledStencilBufferNextFrame = false;

		MeshRendererComponent() = default;
		MeshRendererComponent(const MeshRendererComponent&) = default;
	};
	struct MeshFilterComponent {
		MeshAsset mesh{"",nullptr};

		MeshFilterComponent() = default;
		MeshFilterComponent(const MeshFilterComponent&) = default;
		MeshFilterComponent(const Ref<Mesh>& _mesh) {mesh.mesh = _mesh;}
		MeshFilterComponent(MeshAsset& mesh)
			: mesh(mesh){}
	};
	class Behavior;
	struct NativeBehaviourComponent {
		Behavior* instance = nullptr;
		std::string behaviorName = "";

		Behavior*(*InstantiateBehaviour)();
		void(*DestroyBehaviour)(NativeBehaviourComponent*);

		template<typename T>
		void bind() {
			InstantiateBehaviour = []() { return static_cast<Behavior*>(new T()); };
			DestroyBehaviour = [](NativeBehaviourComponent* behaviour) { delete behaviour->instance; behaviour->instance = nullptr; };
			
			behaviorName = typeid(T).name();

			if (behaviorName.find("::") != std::string::npos) {
				size_t last = behaviorName.find_last_of("::");
				behaviorName.erase(0, last);
			}
		}
	};
}