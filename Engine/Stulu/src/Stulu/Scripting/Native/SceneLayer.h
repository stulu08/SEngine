#pragma once
#include "Stulu/Scene/Scene.h"
#include "Stulu/Scene/YAML.h"

namespace Stulu {
	class STULU_API SceneLayer {
	public:
		virtual bool Initlize(Scene* scene) = 0;
		virtual ~SceneLayer() = default;

		virtual void SceneStart() { }
		virtual void Update() { }
		virtual void PreUpdate() { }
		virtual void SceneExit() {}

		virtual void Render() { }
		virtual void Render2D() { }
		virtual void DrawGizmos() { }

		virtual void GameObjectCreate(const GameObject& object) {}
		virtual void GameObjectDestory(const GameObject& object) {}

		virtual void SerializerGameObject(YAML::Emitter& out, GameObject& gameObject) {}
		virtual void DeserializerGameObject(YAML::detail::iterator_value& gameObject, GameObject& deserialized, const std::string& path) {}
	};
}