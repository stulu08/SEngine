#pragma once
#include "Stulu/Core/UUID.h"
#include "Stulu/Core/Timestep.h"

#include "Stulu/Math/Math.h"

#include "Stulu/Renderer/Renderer2D.h"

#include "Stulu/Scene/AssetsManager.h"
#include "Stulu/Scene/GameObject.h"
#include "Stulu/Scene/Material.h"

#include "Stulu/Scene/Components/Camera.h"

#include "Stulu/Scene/physx/RigidbodyComponent.h"

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
		glm::vec3 position = glm::vec3(0.0f);
		glm::vec3 rotation = glm::vec3(0.0f);
		glm::vec3 scale = glm::vec3(1.0f);

		//this GameObject
		GameObject gameObject = GameObject::null;
		//Parent GameObject
		GameObject parent = GameObject::null;


		TransformComponent() = default;
		TransformComponent(const TransformComponent&) = default;
		TransformComponent(const glm::vec3 pos, const glm::vec3 rotation, const glm::vec3 scale)
			: position(pos), rotation(rotation), scale(scale) { };

		const glm::mat4 getTransform() { return Math::createMat4(position, getOrientation(), scale); }
		const glm::mat4 getTransform() const { return Math::createMat4(position, getOrientation(), scale); }
		const glm::quat getOrientation(){ return glm::quat(rotation); }
		const glm::quat getOrientation() const{ return glm::quat(rotation); }

		const glm::mat4 getWorldSpaceTransform() {
			ST_PROFILING_FUNCTION();
			if (parent) {
				glm::mat4 parentTr = parent.getComponent<TransformComponent>().getWorldSpaceTransform();
				const glm::vec3 ppos = glm::vec3(parentTr[3]);
				parentTr[3] = glm::vec4(0, 0, 0, parentTr[3].w);

				return glm::translate(glm::mat4(1.0f), ppos + position) * glm::toMat4(getOrientation()) * glm::scale(glm::mat4(1.0f), scale) * parentTr;
			}
			return getTransform();
		}

		operator const glm::mat4() { return getWorldSpaceTransform(); }

		glm::vec3 upDirection(){ return glm::rotate(getOrientation(), TRANSFORM_UP_DIRECTION); }
		glm::vec3 rightDirection() { return glm::rotate(getOrientation(), TRANSFORM_RIGHT_DIRECTION); }
		glm::vec3 forwardDirection() { return glm::rotate(getOrientation(), TRANSFORM_FOREWARD_DIRECTION); }

		void addChild(GameObject child) { 
			child.getComponent<TransformComponent>().parent = gameObject;
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

		SkyBoxComponent() = default;
		SkyBoxComponent(const SkyBoxComponent&) = default;
	};

	struct MeshRendererComponent {
		Material* material;

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

	struct BoxColliderComponent {
		float staticFriction = .1f;
		float dynamicFriction = .1f;
		float restitution = .0f;
		glm::vec3 offset = glm::vec3(0.0f);
		glm::vec3 size = glm::vec3(.5f);

		void* shape = nullptr;

		BoxColliderComponent() = default;
		BoxColliderComponent(const BoxColliderComponent&) = default;
	};
	struct SphereColliderComponent {
		float staticFriction = .1f;
		float dynamicFriction = .1f;
		float restitution = .0f;
		glm::vec3 offset = glm::vec3(0.0f);
		float radius = .5f;

		void* shape = nullptr;

		SphereColliderComponent() = default;
		SphereColliderComponent(const SphereColliderComponent&) = default;
	};
	struct MeshColliderComponent {
		float staticFriction = .1f;
		float dynamicFriction = .1f;
		float restitution = .0f;

		MeshColliderComponent() = default;
		MeshColliderComponent(const MeshColliderComponent&) = default;
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