#pragma once
#include "Stulu/Scene/Scene.h"
#include "Stulu/Scene/YAML.h"

namespace Stulu {
	class STULU_API SceneSerializer {
	public:
		SceneSerializer(Ref<Scene>& scene)
			: m_scene(scene){}

		void serialze(const std::string& path);
		bool deSerialze(const std::string& path);

		static std::vector<UUID> getAllSceneAssets(const std::vector<std::string> scenes);
	private:
		void SerializerGameObject(YAML::Emitter& out, GameObject gameObject);

		Ref<Scene> m_scene;
	};
}