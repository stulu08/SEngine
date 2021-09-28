#pragma once
#include "Stulu/Renderer/OrthographicCamera.h"
#include "Stulu/Renderer/PerspectiveCamera.h"
#include "Stulu/Renderer/Renderer2D.h"
#include "Stulu/Scene/GameObject.h"
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
			if (parent) {
				return getTransform() * parent.getComponent<TransformComponent>().getWorldSpaceTransform();
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
	struct CameraComponent {
		enum ClearType { Color = 0, Skybox = 1};
		struct Settings {
			float fov = 80.0f, zoom = 1.0f, zNear = .01f, zFar = 250.0f;
			bool staticAspect = false;
			ClearType clearType = Color;
			//if static aspect
			float aspectRatio = 0.0f;//readonly
			uint32_t textureWidth = 100, textureHeight = 100;
			//if cleaType = Color
			glm::vec4 clearColor = glm::vec4(glm::vec3(.0f),1.0f);
		};
		Ref<Camera> cam = nullptr;
		CameraMode mode = CameraMode::Perspective;
		Settings settings;
		int depth = -1;

		CameraComponent() {
			cam = createRef<PerspectiveCamera>(settings.fov, settings.aspectRatio, settings.zNear, settings.zFar);
		}
		CameraComponent(const CameraComponent&) = default;
		CameraComponent(const CameraMode mode)
			: mode(mode){
			if (mode == CameraMode::Perspective) {
				cam = createRef<PerspectiveCamera>(settings.fov, settings.aspectRatio, settings.zNear, settings.zFar);
			}
			else if (mode == CameraMode::Orthographic) {
				cam = createRef<OrthographicCamera>(-settings.aspectRatio * settings.zoom, settings.aspectRatio * settings.zoom, -settings.zoom, settings.zoom, settings.zNear, settings.zFar);
			}
		}
		const void onResize(uint32_t width, uint32_t height) {
			settings.aspectRatio = (float)width / (float)height;
			settings.textureWidth = width;
			settings.textureHeight = height;
			cam->getFrameBuffer()->resize(width, height);
			updateProjection();
		}
		const void updateSize() {
			settings.aspectRatio = (float)settings.textureWidth / (float)settings.textureHeight;
			cam->getFrameBuffer()->resize(settings.textureWidth, settings.textureHeight);
			updateProjection();
		}
		const void updateProjection() {
			if (mode == CameraMode::Perspective) {
				cam->setProjection(settings.fov, settings.aspectRatio, settings.zNear, settings.zFar);
			}
			else if (mode == CameraMode::Orthographic) {
				cam->setProjection(-settings.aspectRatio * settings.zoom, settings.aspectRatio * settings.zoom, -settings.zoom, settings.zoom, settings.zNear, settings.zFar);
			}
		}
		const void updateMode() {
			if (mode == CameraMode::Orthographic) {
				cam.reset(new OrthographicCamera(-settings.aspectRatio * settings.zoom, settings.aspectRatio * settings.zoom, -settings.zoom, settings.zoom, settings.zNear, settings.zFar));
				cam->getFrameBuffer()->resize(settings.textureWidth, settings.textureHeight);
			}
			else if (mode == CameraMode::Perspective) {
				cam.reset(new PerspectiveCamera(settings.fov, settings.aspectRatio, settings.zNear, settings.zFar));
				cam->getFrameBuffer()->resize(settings.textureWidth, settings.textureHeight);
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

	struct MeshComponent {
		Ref<Shader> shader;
		std::vector<Vertex> vertices;
		std::vector<uint32_t> indices;

		Ref<Stulu::VertexArray> vertexArray = nullptr;

		MeshComponent() = default;
		MeshComponent(const MeshComponent&) = default;
		MeshComponent(const Ref<Shader>& shader)
			: shader(shader) {};
		MeshComponent(const Ref<Shader>& shader, Mesh& mesh)
			: shader(shader) {
			vertices = mesh.getVertices();
			indices = mesh.getIndices();
			recalculate();
		};
		void recalculate() {
			Stulu::Ref<Stulu::VertexBuffer> vertexBuffer;
			Stulu::Ref<Stulu::IndexBuffer> indexBuffer;

			vertexArray = Stulu::VertexArray::create();
			vertexBuffer = Stulu::VertexBuffer::create((uint32_t)(vertices.size() * sizeof(Vertex)), &vertices[0]);
			vertexBuffer->setLayout(Mesh::getDefaultLayout());
			vertexArray->addVertexBuffer(vertexBuffer);
			indexBuffer = Stulu::IndexBuffer::create((uint32_t)indices.size(), indices.data());
			vertexArray->setIndexBuffer(indexBuffer);
		}
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