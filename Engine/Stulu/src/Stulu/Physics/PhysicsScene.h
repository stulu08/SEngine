#pragma once
#include "PhysicsHelper.h"
#include "PhysicsModule.h"

#include "Stulu/Scripting/Native/SceneLayer.h"

namespace Stulu {
	class STULU_API Scene;

	class STULU_API PhysicsScene : public SceneLayer {
	public:
		inline virtual ~PhysicsScene() {
			SceneExit();
		}

		virtual bool Initlize(Scene* scene) override;

		virtual void SceneStart() override;
		virtual void PreUpdate() override;
		virtual void SceneExit() override;
		virtual void GameObjectDestory(const GameObject& object) override;

		virtual void SerializerGameObject(YAML::Emitter& out, GameObject& gameObject) override;
		virtual void DeserializerGameObject(YAML::detail::iterator_value& gameObject, GameObject& deserialized, const std::string& path) override;

		virtual void SerializerScene(YAML::Emitter& out) override;
		virtual void DeserializerScene(YAML::Node& data) override;

		inline bool IsValid() const { return m_enabled && m_physicsScene; }
		inline physx::PxScene* GetPhysicsScene() const { return m_physicsScene; }
		inline physx::PxPhysics* GetContext() const { return PhysicsModule::Get().GetPhysics(); }

		void SetDebugVisual(PhysicsHelper::DebugViuals option, float value) const;
		float GetDebugVisual(PhysicsHelper::DebugViuals option) const;
		// Call inside gizom renderer
		void RenderSceneDebugData() const;

		const glm::vec3& GetGravity() const { return m_gravity; }
		void SetGravity(const glm::vec3& value);

		void CreatePhysicsObject();
	private:
		physx::PxScene* m_physicsScene = nullptr;
		physx::PxControllerManager* m_controllerManager = nullptr;
		Scene* m_scene = nullptr;
		bool m_enabled = false;
		float m_time = 0.0f;

		glm::vec3 m_gravity = { 0.0f, -9.81f, 0.0f };
	};
}