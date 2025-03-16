#include "st_pch.h"
#include "Stulu.h"

namespace Stulu {
	std::unordered_map<size_t, std::function<void(entt::registry&, entt::registry&, Scene*)>> Component::m_componentCopyList;
	
	void Component::RegisterBaseComponents() {
		Register<GameObjectBaseComponent>("Stulu.GameObjectBaseComponent");
		Register<TransformComponent>("Stulu.TransformComponent");

		Register<SpriteRendererComponent>("Stulu.SpriteRendererComponent");
		Register<CircleRendererComponent>("Stulu.CircleRendererComponent");
		
		Register<LightComponent>("Stulu.LightComponent");
		Register<MeshRendererComponent>("Stulu.MeshRendererComponent");
		Register<MeshFilterComponent>("Stulu.MeshFilterComponent");
		
		Register<SkyBoxComponent>("Stulu.SkyBoxComponent");
		Register<PostProcessingComponent>("Stulu.PostProcessingComponent");
		Register<CameraComponent>("Stulu.CameraComponent");
		
		Register<RigidbodyComponent>("Stulu.RigidbodyComponent");
		Register<RigidStaticComponent>("Stulu.RigidStaticComponent");
		Register<BoxColliderComponent>("Stulu.BoxColliderComponent");
		Register<SphereColliderComponent>("Stulu.SphereColliderComponent");
		Register<CapsuleColliderComponent>("Stulu.CapsuleColliderComponent");
		Register<MeshColliderComponent>("Stulu.MeshColliderComponent");
		
		Register<ScriptingComponent>("Stulu.ScriptingComponent");
	}
}