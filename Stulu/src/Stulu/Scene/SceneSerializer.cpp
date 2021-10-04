#include "st_pch.h"
#include "SceneSerializer.h"

#include <yaml-cpp/yaml.h>

#include "Stulu/Scene/Components.h"
namespace YAML {

	template<>
	struct convert<glm::vec2> {
		static Node encode(const glm::vec2& rhs) {
			Node node;
			node.push_back(rhs.x);
			node.push_back(rhs.y);
			node.SetStyle(EmitterStyle::Flow);
			return node;
		}

		static bool decode(const Node& node, glm::vec2& rhs) {
			if (!node.IsSequence() || node.size() != 2)
				return false;

			rhs.x = node[0].as<float>();
			rhs.y = node[1].as<float>();
			return true;
		}
	};

	template<>
	struct convert<glm::vec3> {
		static Node encode(const glm::vec3& rhs) {
			Node node;
			node.push_back(rhs.x);
			node.push_back(rhs.y);
			node.push_back(rhs.z);
			node.SetStyle(EmitterStyle::Flow);
			return node;
		}

		static bool decode(const Node& node, glm::vec3& rhs) {
			if (!node.IsSequence() || node.size() != 3)
				return false;

			rhs.x = node[0].as<float>();
			rhs.y = node[1].as<float>();
			rhs.z = node[2].as<float>();
			return true;
		}
	};

	template<>
	struct convert<glm::vec4> {
		static Node encode(const glm::vec4& rhs) {
			Node node;
			node.push_back(rhs.x);
			node.push_back(rhs.y);
			node.push_back(rhs.z);
			node.push_back(rhs.w);
			node.SetStyle(EmitterStyle::Flow);
			return node;
		}

		static bool decode(const Node& node, glm::vec4& rhs) {
			if (!node.IsSequence() || node.size() != 4)
				return false;

			rhs.x = node[0].as<float>();
			rhs.y = node[1].as<float>();
			rhs.z = node[2].as<float>();
			rhs.w = node[3].as<float>();
			return true;
		}
	};

}
namespace Stulu {
	YAML::Emitter& operator<<(YAML::Emitter& out, const glm::vec2& v) {
		out << YAML::Flow;
		out << YAML::BeginSeq << v.x << v.y << YAML::EndSeq;
		return out;
	}
	YAML::Emitter& operator<<(YAML::Emitter& out, const glm::vec3& v) {
		out << YAML::Flow;
		out << YAML::BeginSeq << v.x << v.y << v.z << YAML::EndSeq;
		return out;
	}
	YAML::Emitter& operator<<(YAML::Emitter& out, const glm::vec4& v) {
		out << YAML::Flow;
		out << YAML::BeginSeq << v.x << v.y << v.z << v.w << YAML::EndSeq;
		return out;
	}
	static void SerializerGameObject(YAML::Emitter& out, GameObject gameObject) {
		out << YAML::BeginMap;
		out << YAML::Key << "GameObject" << YAML::Value << "12837192831273"; // TODO: Entity ID goes here

		if (gameObject.hasComponent<GameObjectBaseComponent>()) {
			out << YAML::Key << "GameObjectBaseComponent";
			out << YAML::BeginMap;
			std::string& tag = gameObject.getComponent<GameObjectBaseComponent>().tag;
			std::string& name = gameObject.getComponent<GameObjectBaseComponent>().name;
			out << YAML::Key << "name" << YAML::Value << name;
			out << YAML::Key << "tag" << YAML::Value << tag;
			out << YAML::EndMap;
		}

		if (gameObject.hasComponent<TransformComponent>()) {
			out << YAML::Key << "TransformComponent";
			out << YAML::BeginMap;
			auto& trans = gameObject.getComponent<TransformComponent>();
			out << YAML::Key << "position" << YAML::Value << trans.position;
			out << YAML::Key << "rotation" << YAML::Value << trans.rotation;
			out << YAML::Key << "scale" << YAML::Value << trans.scale;
			out << YAML::EndMap;
		}

		if (gameObject.hasComponent<CameraComponent>()) {
			out << YAML::Key << "CameraComponent";
			out << YAML::BeginMap;
			auto& camera = gameObject.getComponent<CameraComponent>();
			auto& settings = camera.settings;
			out << YAML::Key << "Camera Settings" << YAML::Value;
			out << YAML::BeginMap;
			out << YAML::Key << "clearType" << YAML::Value << (int)settings.clearType;
			out << YAML::Key << "clearColor" << YAML::Value << settings.clearColor;
			out << YAML::Key << "fov" << YAML::Value << settings.fov;
			out << YAML::Key << "zoom" << YAML::Value << settings.zoom;
			out << YAML::Key << "zNear" << YAML::Value << settings.zNear;
			out << YAML::Key << "zFar" << YAML::Value << settings.zFar;
			out << YAML::Key << "staticAspect" << YAML::Value << settings.staticAspect;
			out << YAML::Key << "aspectRatio" << YAML::Value << settings.aspectRatio;
			out << YAML::Key << "textureWidth" << YAML::Value << settings.textureWidth;
			out << YAML::Key << "textureHeight" << YAML::Value << settings.textureHeight;
			out << YAML::EndMap;
			out << YAML::Key << "mode" << YAML::Value << (int)camera.mode;
			out << YAML::Key << "depth" << YAML::Value << camera.depth;
			out << YAML::EndMap;
		}

		if (gameObject.hasComponent<SpriteRendererComponent>()) {
			out << YAML::Key << "SpriteRendererComponent";
			out << YAML::BeginMap;
			auto& spriteRendererComponent = gameObject.getComponent<SpriteRendererComponent>();
			out << YAML::Key << "color" << YAML::Value << spriteRendererComponent.color;
			out << YAML::Key << "tiling" << YAML::Value << spriteRendererComponent.tiling;
			out << YAML::EndMap;
		}
		if (gameObject.hasComponent<LightComponent>()) {
			out << YAML::Key << "LightComponent";
			out << YAML::BeginMap;
			auto& light = gameObject.getComponent<LightComponent>();
			out << YAML::Key << "color" << YAML::Value << light.color;
			out << YAML::Key << "lightType" << YAML::Value << (int)light.lightType;
			out << YAML::Key << "strength" << YAML::Value << light.strength;
			out << YAML::Key << "spotLight_cutOff" << YAML::Value << light.spotLight_cutOff;
			out << YAML::Key << "spotLight_outerCutOff" << YAML::Value << light.spotLight_outerCutOff;
			out << YAML::EndMap;
		}

		out << YAML::EndMap;
	}

	void SceneSerializer::serialze(const std::string& path) {

		YAML::Emitter out;
		out << YAML::BeginMap;
		out << YAML::Key << "Scene" << YAML::Value << "Untitled";
		out << YAML::Key << "GameObjects" << YAML::Value << YAML::BeginSeq;
		m_scene->m_registry.each([&](entt::entity id) {
				GameObject go = { id, m_scene.get() };
				if (go == GameObject::null)
					return;
				
				if (!go.getComponent<TransformComponent>().parent)
					SerializerGameObject(out, go);
				else
					CORE_WARN("Child object saving currently not supported");
			});
		out << YAML::EndSeq;
		out << YAML::EndMap;

		FILE* file = fopen(path.c_str(), "w");
		fprintf(file, out.c_str());
		fclose(file);

	}
	void SceneSerializer::deSerialze(const std::string& path) {
		/*FILE* file = fopen(path.c_str(), "rb");
		fseek(file, 0, SEEK_END);
		long fileSize = ftell(file);
		fseek(file, 0, SEEK_SET);
		char* fileContent = (char*)malloc(fileSize + 1);
		fread(fileContent, 1, fileSize, file);
		fclose(file);
		fileContent[fileSize] = 0;*/

		YAML::Node data = YAML::LoadFile(path);
		std::string name = data["Scene"].as<std::string>();

		auto gos = data["GameObjects"];
		if (gos) {
			for (auto gameObject : gos) {
				uint64_t uuid = gameObject["GameObject"].as<uint64_t>();

				std::string name;
				std::string tag;
				auto base = gameObject["GameObjectBaseComponent"];
				if (base) {
					tag = base["tag"].as<std::string>();
					name = base["name"].as<std::string>();
				}


				GameObject deserialized = m_scene->createGameObject(name);
				deserialized.getComponent<GameObjectBaseComponent>().tag = tag;
				auto transformComponent = gameObject["TransformComponent"];
				if (transformComponent) {
					auto& tc = deserialized.getComponent<TransformComponent>();
					tc.position = transformComponent["position"].as<glm::vec3>();
					tc.rotation = transformComponent["rotation"].as<glm::vec3>();
					tc.scale = transformComponent["scale"].as<glm::vec3>();
				}

				auto cameraComponent = gameObject["CameraComponent"];
				if (cameraComponent) {
					auto& cc = deserialized.addComponent<CameraComponent>();
					auto& settings = cameraComponent["Camera Settings"];

					cc.settings.clearType = (CameraComponent::ClearType)settings["clearType"].as<int>();
					cc.settings.clearColor = settings["clearColor"].as<glm::vec4>();
					cc.settings.fov = settings["fov"].as<float>();
					cc.settings.zoom = settings["zoom"].as<float>();
					cc.settings.zNear = settings["zNear"].as<float>();
					cc.settings.zFar = settings["zFar"].as<float>();
					cc.settings.staticAspect = settings["staticAspect"].as<bool>();
					cc.settings.aspectRatio = settings["aspectRatio"].as<float>();
					cc.settings.textureWidth = settings["textureWidth"].as<uint32_t>();
					cc.settings.textureHeight = settings["textureHeight"].as<uint32_t>();


					cc.mode = (CameraMode)cameraComponent["mode"].as<int>();
					cc.depth = (CameraMode)cameraComponent["depth"].as<int>();
				}

				auto spriteRendererComponent = gameObject["SpriteRendererComponent"];
				if (spriteRendererComponent) {
					auto& src = deserialized.addComponent<SpriteRendererComponent>();
					src.color = spriteRendererComponent["color"].as<glm::vec4>();
					src.tiling = spriteRendererComponent["tiling"].as<glm::vec2>();
				}

				auto lightC = gameObject["LightComponent"];
				if (lightC) {
					auto& light = deserialized.addComponent<LightComponent>();
					light.color = lightC["color"].as<glm::vec3>();
					light.lightType = (LightComponent::Type)lightC["lightType"].as<int>();
					light.strength = lightC["strength"].as<float>();
					light.spotLight_cutOff = lightC["spotLight_cutOff"].as<float>();
					light.spotLight_outerCutOff = lightC["spotLight_outerCutOff"].as<float>();
				}
			}
		}
	}
}