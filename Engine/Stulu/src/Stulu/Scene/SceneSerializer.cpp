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
			try {
				deSerializerFunc(node, gameObject);
			}
			catch (YAML::Exception ex) {
				CORE_ERROR("YAML Expception while deserializing {0} on Object {1} :\n{1}", Component::m_componentTable[type].first, gameObject.GetID(), ex.what());
			}

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
				out << YAML::Key << "enablePhsyics3D" << YAML::Value << scene->getData().enablePhsyics3D;

				out << YAML::Key << "Shadows" << YAML::Value << YAML::BeginMap;
				out << YAML::Key << "MapSize" << YAML::Value << scene->getData().graphicsData.shadows.MapSize;
				out << YAML::Key << "CascadeCount" << YAML::Value << scene->getData().graphicsData.shadows.CascadeSplits;
				out << YAML::Key << "NearPlane" << YAML::Value << scene->getData().graphicsData.shadows.NearPlane;
				out << YAML::Key << "FarPlane" << YAML::Value << scene->getData().graphicsData.shadows.FarPlane;
				out << YAML::Key << "BlendingDistance" << YAML::Value << scene->getData().graphicsData.shadows.BlendingDistance;
				out << YAML::Key << "SampleQuality" << YAML::Value << (uint32_t)scene->getData().graphicsData.shadows.SampleQuality;
				out << YAML::Key << "PCSSQuality" << YAML::Value << (uint32_t)scene->getData().graphicsData.shadows.PCSSQuality;
				out << YAML::Key << "ZMult" << YAML::Value << scene->getData().graphicsData.shadows.ZMult;
				out << YAML::EndMap;

				out << YAML::Key << "Fog" << YAML::Value << YAML::BeginMap;
				out << YAML::Key << "fogMode" << YAML::Value << scene->getData().graphicsData.fog.fogMode;
				out << YAML::Key << "fogColor" << YAML::Value << scene->getData().graphicsData.fog.fogColor;
				out << YAML::Key << "linearFogStart" << YAML::Value << scene->getData().graphicsData.fog.linearFogStart;
				out << YAML::Key << "linearFogEnd" << YAML::Value << scene->getData().graphicsData.fog.linearFogEnd;
				out << YAML::Key << "fogDensity" << YAML::Value << scene->getData().graphicsData.fog.fogDensity;
				out << YAML::Key << "FogHorizonStrength" << YAML::Value << scene->getData().graphicsData.fog.FogHorizonStrength;
				out << YAML::Key << "FogHorizonFalloff" << YAML::Value << scene->getData().graphicsData.fog.FogHorizonFalloff;
				out << YAML::Key << "EnableFogHorizon" << YAML::Value << scene->getData().graphicsData.fog.EnableFogHorizon;
				out << YAML::Key << "FogGroundStrength" << YAML::Value << scene->getData().graphicsData.fog.FogGroundStrength;
				out << YAML::Key << "EnableGroundFog" << YAML::Value << scene->getData().graphicsData.fog.EnableGroundFog;
				out << YAML::Key << "FogHorizonOffset" << YAML::Value << scene->getData().graphicsData.fog.FogHorizonOffset;
				out << YAML::Key << "FogHorizonHeightFalloff" << YAML::Value << scene->getData().graphicsData.fog.FogHorizonHeightFalloff;
				out << YAML::Key << "EnableHorizonHeightFog" << YAML::Value << scene->getData().graphicsData.fog.EnableHorizonHeightFog;
				out << YAML::EndMap;

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
				if (settings["enablePhsyics3D"]) 
					sceneData.enablePhsyics3D = settings["enablePhsyics3D"].as<bool>();

				if (settings["Shadows"]) {
					YAML::Node shadows = settings["Shadows"];
					if (shadows["MapSize"])
						sceneData.graphicsData.shadows.MapSize = shadows["MapSize"].as<uint32_t>();
					if (shadows["CascadeCount"])
						sceneData.graphicsData.shadows.CascadeSplits = shadows["CascadeCount"].as<std::vector<float>>();
					if (shadows["NearPlane"])
						sceneData.graphicsData.shadows.NearPlane = shadows["NearPlane"].as<float>();
					if (shadows["FarPlane"])
						sceneData.graphicsData.shadows.FarPlane = shadows["FarPlane"].as<float>();
					if (shadows["BlendingDistance"])
						sceneData.graphicsData.shadows.BlendingDistance = shadows["BlendingDistance"].as<float>();
					if (shadows["SampleQuality"])
						sceneData.graphicsData.shadows.SampleQuality = shadows["SampleQuality"].as<uint32_t>();
					if (shadows["PCSSQuality"])
						sceneData.graphicsData.shadows.PCSSQuality = shadows["PCSSQuality"].as<uint32_t>();
					if (shadows["ZMult"])
						sceneData.graphicsData.shadows.ZMult = shadows["ZMult"].as<float>();

					scene->getRenderer()->resizeShadowMap();
				}

				if (settings["Fog"]) {
					YAML::Node fog = settings["Fog"];
					if(fog["fogColor"])
						sceneData.graphicsData.fog.fogColor = fog["fogColor"].as<glm::vec4>();
					if(fog["fogMode"])
						sceneData.graphicsData.fog.fogMode = fog["fogMode"].as<float>();
					if(fog["linearFogStart"])
						sceneData.graphicsData.fog.linearFogStart = fog["linearFogStart"].as<float>();
					if(fog["linearFogEnd"])
						sceneData.graphicsData.fog.linearFogEnd = fog["linearFogEnd"].as<float>();
					if(fog["fogDensity"])
						sceneData.graphicsData.fog.fogDensity = fog["fogDensity"].as<float>();
					if(fog["FogHorizonStrength"])
						sceneData.graphicsData.fog.FogHorizonStrength = fog["FogHorizonStrength"].as<float>();
					if(fog["FogHorizonFalloff"])
						sceneData.graphicsData.fog.FogHorizonFalloff = fog["FogHorizonFalloff"].as<float>();
					if(fog["EnableFogHorizon"])
						sceneData.graphicsData.fog.EnableFogHorizon = fog["EnableFogHorizon"].as<float>();
					if(fog["FogGroundStrength"])
						sceneData.graphicsData.fog.FogGroundStrength = fog["FogGroundStrength"].as<float>();
					if(fog["EnableGroundFog"])
						sceneData.graphicsData.fog.EnableGroundFog = fog["EnableGroundFog"].as<float>();
					if(fog["FogHorizonOffset"])
						sceneData.graphicsData.fog.FogHorizonOffset = fog["FogHorizonOffset"].as<float>();
					if(fog["FogHorizonHeightFalloff"])
						sceneData.graphicsData.fog.FogHorizonHeightFalloff = fog["FogHorizonHeightFalloff"].as<float>();
					if (fog["EnableHorizonHeightFog"])
						sceneData.graphicsData.fog.EnableHorizonHeightFog = fog["EnableHorizonHeightFog"].as<float>();
				}

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