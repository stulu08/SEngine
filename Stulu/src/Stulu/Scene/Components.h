#pragma once
#include "Stulu/Renderer/OrthographicCamera.h"
#include "Stulu/Renderer/PerspectiveCamera.h"
#include "Stulu/Renderer/Renderer2D.h"
#include "Stulu/Renderer/Model.h"
#include "Stulu/Core/Timestep.h"
#include "Stulu/Math/Math.h"


namespace Stulu {
	struct GameObjectBaseComponent {
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

		TransformComponent() = default;
		TransformComponent(const TransformComponent&) = default;
		TransformComponent(const glm::vec3 pos, const glm::vec3 rotation, const glm::vec3 scale)
			: position(pos), rotation(rotation), scale(scale) { };

		const glm::mat4 getTransform() { return Math::createMat4(position, rotation, scale); }
		const glm::mat4 getTransform() const { return Math::createMat4(position, rotation, scale); }
		const glm::quat getOrientation(){ return glm::quat(glm::radians(rotation)); }

		operator const glm::mat4() { return getTransform(); }
		operator const glm::mat4() const { return getTransform(); }

		glm::vec3 upDirection(){ return glm::rotate(getOrientation(), TRANSFORM_UP_DIRECTION); }
		glm::vec3 rightDirection() { return glm::rotate(getOrientation(), TRANSFORM_RIGHT_DIRECTION); }
		glm::vec3 forwardDirection() { return glm::rotate(getOrientation(), TRANSFORM_FOREWARD_DIRECTION); }

	};
	struct CameraComponent {
		struct Settings {
			float fov = 80.0f, zoom = 1.0f, zNear = .01f, zFar = 250.0f;
			bool staticAspect = false;
		};
		Ref<Camera> cam = nullptr;
		CameraMode mode = CameraMode::Perspective;
		Settings settings;
		int depth = -1;
		float aspectRatio = 0.0f;

		CameraComponent() {
			cam = createRef<PerspectiveCamera>(settings.fov, aspectRatio, settings.zNear, settings.zFar);
		}
		CameraComponent(const CameraComponent&) = default;
		CameraComponent(const CameraMode mode)
			: mode(mode){
			if (mode == CameraMode::Perspective) {
				cam = createRef<PerspectiveCamera>(settings.fov, aspectRatio, settings.zNear, settings.zFar);
			}
			else if (mode == CameraMode::Orthographic) {
				cam = createRef<OrthographicCamera>(-aspectRatio * settings.zoom, aspectRatio * settings.zoom, -settings.zoom, settings.zoom, settings.zNear, settings.zFar);
			}
		}
		const void onResize(uint32_t width, uint32_t height) {
			aspectRatio = (float)width / (float)height;
			//cam->resizeFrameBuffer(width, height);
			updateProjection();
		}
		const void updateProjection() {
			if (mode == CameraMode::Perspective) {
				cam->setProjection(settings.fov, aspectRatio, settings.zNear, settings.zFar);
			}
			else if (mode == CameraMode::Orthographic) {
				cam->setProjection(-aspectRatio * settings.zoom, aspectRatio * settings.zoom, -settings.zoom, settings.zoom, settings.zNear, settings.zFar);
			}
		}
		const void updateMode() {
			if (mode == CameraMode::Perspective) {
				cam = createRef<PerspectiveCamera>(settings.fov, aspectRatio, settings.zNear, settings.zFar);
				cam->setProjection(settings.fov, aspectRatio, settings.zNear, settings.zFar);
			}
			else if (mode == CameraMode::Orthographic) {
				cam = createRef<OrthographicCamera>(-aspectRatio * settings.zoom, aspectRatio * settings.zoom, -settings.zoom, settings.zoom, settings.zNear, settings.zFar);
				cam->setProjection(-aspectRatio * settings.zoom, aspectRatio * settings.zoom, -settings.zoom, settings.zoom, settings.zNear, settings.zFar);
			}
		}
	};

	struct LightComponent {
		enum Type{ Directional = 0, Area = 1, Spot = 2 };

		Type lightType = Type::Directional;
		glm::vec3 color = glm::vec3(1.0f);
		float stength = 1.0f;

		float spotLight_cutOff = 10.0f;
		float spotLight_outerCutOff = 15.0f;

		LightComponent() = default;
		LightComponent(const LightComponent&) = default;
		LightComponent(const Type& type)
			: lightType(type) {};
	};

	struct ModelRendererComponent {
		Ref<Shader> shader;
		Model model;
		ModelRendererComponent() = default;
		ModelRendererComponent(const ModelRendererComponent&) = default;
		ModelRendererComponent(const Model& model, const Ref<Shader>& shader)
			: model(model), shader(shader) {};
	};

	struct SpriteRendererComponent {
		glm::vec4 color = COLOR_WHITE;
		SpriteRendererComponent() = default;
		SpriteRendererComponent(const SpriteRendererComponent&) = default;
		SpriteRendererComponent(const glm::vec4 color)
			: color(color) {};
	};

	class Behavior;
	struct NativeBehaviourComponent {
		Behavior* instance = nullptr;
		std::string behaviorName = "";

		Behavior*(*InstantiateBehaviour)();
		void(*DestroyBehaviour)(NativeBehaviourComponent*);

		template<typename T>
		void bind() {
			InstantiateBehaviour = []() { return static_cast<Behavior*>(new T); };
			DestroyBehaviour = [](NativeBehaviourComponent* behaviour) { delete behaviour->instance; behaviour->instance = nullptr; };
			
			behaviorName = typeid(T).name();

			if (behaviorName.find("::") != std::string::npos) {
				size_t last = behaviorName.find_last_of("::");
				behaviorName.erase(0, last);
			}
		}
	};
}