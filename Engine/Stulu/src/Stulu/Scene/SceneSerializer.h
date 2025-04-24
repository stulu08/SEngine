#pragma once
#include "Stulu/Scene/Scene.h"
#include "Stulu/Serialization/YAML.h"

namespace Stulu {
	class STULU_API RegistrySerializer {
	public:
		RegistrySerializer(Registry* registry)
			: m_registry(registry) {}

		void SerializeGameObject(YAML::Emitter& out, GameObject gameObject);
		void DeserializeGameObject(YAML::Node& node, GameObject gameObject);
		
		bool SerialzeRegistry(const std::string& path);
		bool DeserialzeRegistry(const std::string& path);
	protected:
		Registry* m_registry;

		void SerializeGameObjects(YAML::Emitter& out);
		void DeserializeGameObjects(YAML::Node& node);
	};

	class STULU_API SceneSerializer : public RegistrySerializer {
	public:
		SceneSerializer(Scene* scene)
			: RegistrySerializer(scene){}

		void Serialze(const std::string& path);
		bool Deserialze(const std::string& path);

		Scene* GetScene() { return m_registry->GetAsScene(); }
	};
}