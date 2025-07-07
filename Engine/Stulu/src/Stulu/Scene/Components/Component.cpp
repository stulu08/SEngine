#include "st_pch.h"
#include "Stulu.h"

namespace Stulu {
	std::unordered_map<size_t, std::function<void(entt::registry&, entt::registry&, Scene*)>> Component::m_componentCopyList;
	std::unordered_map<size_t, std::function<void(YAML::Emitter&, GameObject)>> Component::m_componentSerializeList;
	std::unordered_map<size_t, std::function<void(YAML::Node&, GameObject)>> Component::m_componentDeserializeList;
	std::unordered_map<size_t, std::pair<std::string, std::string>> Component::m_componentTable;
	
	void Component::RegisterBaseComponents() {
		Register<GameObjectBaseComponent>("GameObjectBaseComponent", "Stulu.GameObjectBaseComponent");
		Register<TransformComponent>("TransformComponent", "Stulu.TransformComponent");

		Register<SpriteRendererComponent>("SpriteRendererComponent", "Stulu.SpriteRendererComponent");
		Register<CircleRendererComponent>("CircleRendererComponent", "Stulu.CircleRendererComponent");
		
		Register<LightComponent>("LightComponent", "Stulu.LightComponent");
		Register<MeshRendererComponent>("MeshRendererComponent", "Stulu.MeshRendererComponent");
		Register<MeshFilterComponent>("MeshFilterComponent", "Stulu.MeshFilterComponent");
		
		Register<SkyBoxComponent>("SkyBoxComponent", "Stulu.SkyBoxComponent");
		Register<PostProcessingComponent>("PostProcessingComponent", "Stulu.PostProcessingComponent");
		Register<CameraComponent>("CameraComponent", "Stulu.CameraComponent");
		
		Register<ScriptingComponent>("ScriptingComponent", "Stulu.ScriptingComponent");
	}
}