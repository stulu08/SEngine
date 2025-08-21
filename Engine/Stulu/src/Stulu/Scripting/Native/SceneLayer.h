#pragma once
#include "Stulu/Scene/Scene.h"
#include "Stulu/Serialization/YAML.h"

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

		virtual void SerializerScene(YAML::Emitter& out) {}
		virtual void DeserializerScene(YAML::Node& data) {}
	};
}