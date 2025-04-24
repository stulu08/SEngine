#include "st_pch.h"
#include "SceneSerializer.h"

#include "Stulu.h"
#include <Stulu/Physics/PhysicsScene.h>


namespace Stulu {

	void RegistrySerializer::SerializeGameObject(YAML::Emitter& out, GameObject gameObject) {
		out << YAML::BeginMap;
		out << YAML::Key << "GameObject" << YAML::Value << (uint64_t)gameObject.GetID();
		for (const auto& [type, serializerFunc] : Component::m_componentSerializeList) {
			serializerFunc(out, gameObject);
		}
		out << YAML::EndMap;
	}
	void RegistrySerializer::DeserializeGameObject(YAML::Node& node, GameObject gameObject) {
		for (const auto& [type, deSerializerFunc] : Component::m_componentDeserializeList) {
			deSerializerFunc(node, gameObject);
		}
	}

	void RegistrySerializer::SerializeGameObjects(YAML::Emitter& out) {
		out << YAML::Key << "GameObjects" << YAML::Value << YAML::BeginSeq;
		for (auto [id, comp] : m_registry->GetRegistry().storage<GameObjectBaseComponent>().each()) {
			GameObject go = { id, m_registry };
			if (!go.IsValid())
				return;
			SerializeGameObject(out, go);
		}
		out << YAML::EndSeq;
	}

	void RegistrySerializer::DeserializeGameObjects(YAML::Node& node) {
		auto gameObjects = node["GameObjects"];
		if (gameObjects) {
			for (auto gameObject : gameObjects) {
				GameObject deserialized = m_registry->Create(gameObject["GameObject"].as<entt::entity>());
				DeserializeGameObject(gameObject, deserialized);
			}

			//handle childs and parents
			for (auto gameObject : gameObjects) {
				GameObject deserialized = GameObject::GetById((entt::entity)gameObject["GameObject"].as<uint64_t>(), m_registry);

				auto transformComponentNode = gameObject[Component::GetNativeComponentName<TransformComponent>()];
				if (transformComponentNode) {
					if (transformComponentNode["parent"]) {
						GameObject parentObject = GameObject::GetById((entt::entity)transformComponentNode["parent"].as<uint64_t>(), m_registry);
						deserialized.getComponent<TransformComponent>().SetParent(parentObject);
					}
				}
			}
		}
	}

	bool RegistrySerializer::SerialzeRegistry(const std::string& path) {
		Registry* backupRegistry = StuluBindings::GetCurrentRegistry();
		StuluBindings::SetCurrentRegistry(m_registry);

		YAML::Emitter out;
		out << YAML::BeginMap;
		{
			SerializeGameObjects(out);
		}
		out << YAML::EndMap;

		FILE* file = fopen(path.c_str(), "w");
		fprintf(file, out.c_str());
		fclose(file);

		StuluBindings::SetCurrentRegistry(backupRegistry);
		return true;
	}

	bool RegistrySerializer::DeserialzeRegistry(const std::string& path) {
		if (!FileExists(path.c_str()))
			return false;

		Registry* backupRegistry = StuluBindings::GetCurrentRegistry();
		StuluBindings::SetCurrentRegistry(m_registry);
		try {
			YAML::Node Data = YAML::LoadFile(path);
			DeserializeGameObjects(Data);
			return true;
		}
		catch (YAML::Exception ex) {
			StuluBindings::SetCurrentRegistry(backupRegistry);
			CORE_ERROR("{1}: YAML exception at {2}\n{0}", ex.what(), path, ex.mark.line);
			return false;
		}

		StuluBindings::SetCurrentRegistry(backupRegistry);
		return true;

	}

	void SceneSerializer::Serialze(const std::string& path) {
		Registry* backupRegistry = StuluBindings::GetCurrentRegistry();
		StuluBindings::SetCurrentRegistry(m_registry);

		YAML::Emitter out;
		out << YAML::BeginMap;
		{
			Scene* scene = GetScene();

			out << YAML::Key << "Scene" << YAML::Value << "Untitled";
			out << YAML::Key << "Settings" << YAML::Value << YAML::BeginMap;
			{
				out << YAML::Key << "shaderFlags" << YAML::Value << scene->getData().shaderFlags;
				out << YAML::Key << "env_lod" << YAML::Value << scene->getData().graphicsData.env_lod;
				out << YAML::Key << "shadowDistance" << YAML::Value << scene->getData().graphicsData.shadowDistance;
				out << YAML::Key << "shadowFar" << YAML::Value << scene->getData().graphicsData.shadowFar;
				out << YAML::Key << "shadowMapSize" << YAML::Value << scene->getData().graphicsData.shadowMapSize;
				out << YAML::Key << "enablePhsyics3D" << YAML::Value << scene->getData().enablePhsyics3D;
			}
			out << YAML::EndMap;
			scene->getCaller()->SerializerScene(out);

			SerializeGameObjects(out);
		}
		out << YAML::EndMap;

		FILE* file = fopen(path.c_str(), "w");
		fprintf(file, out.c_str());
		fclose(file);

		StuluBindings::SetCurrentRegistry(backupRegistry);
	}

	bool SceneSerializer::Deserialze(const std::string& path) {
		if (!FileExists(path.c_str()))
			return false;

		Registry* backupRegistry = StuluBindings::GetCurrentRegistry();
		StuluBindings::SetCurrentRegistry(m_registry);

		try {
			YAML::Node Data = YAML::LoadFile(path);
			Scene* scene = GetScene();

			if (Data["Settings"]) {
				YAML::Node settings = Data["Settings"];
				auto& sceneData = scene->getData();

				if (settings["shaderFlags"]) 
					sceneData.shaderFlags = settings["shaderFlags"].as<uint32_t>();
				if (settings["env_lod"]) 
					sceneData.graphicsData.env_lod = settings["env_lod"].as<float>();
				if (settings["shadowDistance"]) 
					sceneData.graphicsData.shadowDistance = settings["shadowDistance"].as<float>();
				if (settings["shadowFar"]) 
					sceneData.graphicsData.shadowFar = settings["shadowFar"].as<float>();
				if (settings["shadowMapSize"]) 
					sceneData.graphicsData.shadowMapSize = settings["shadowMapSize"].as<uint32_t>();
				if (settings["enablePhsyics3D"]) 
					sceneData.enablePhsyics3D = settings["enablePhsyics3D"].as<bool>();

			}
			scene->getCaller()->DeserializerScene(Data);
			
			DeserializeGameObjects(Data);

			return true;
		}
		catch (YAML::Exception ex) {
			StuluBindings::SetCurrentRegistry(backupRegistry);
			CORE_ERROR("{1}: YAML exception at {2}\n{0}", ex.what(), path, ex.mark.line);
			return false;
		}

		StuluBindings::SetCurrentRegistry(backupRegistry);
		return true;
	}
}