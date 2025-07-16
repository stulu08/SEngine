#pragma once
#include "Stulu/Events/KeyEvent.h" 
#include "Stulu/Events/MouseEvent.h"
#include "Stulu/Renderer/Buffer.h"
#include "Stulu/Renderer/Camera.h"
#include "Stulu/Renderer/Shader.h"
#include "Stulu/Renderer/Renderer.h"
#include "Stulu/Scene/Model.h"
#include "Stulu/Types/Timestep.h"
#include "Registry.h"

namespace Stulu {
	struct ShadowSettings {
		uint32_t MapSize = 2048u;
		std::vector<float> CascadeSplits = Math::CalculateCascadeSplits(3, 500.0f);
		float NearPlane = 0.01f;
		float FarPlane = 500.0f;
		float ZMult = 10.0f;
		float BlendingDistance = 10.0f;
		// Poisson sample count: 0->8, 1->16, 2->32, 3->64
		uint8_t SampleQuality = 1;
	};
	struct SceneGraphicsData {
		float env_lod = 1.0f;
		// this wont render the skybox, but will still bind it
		bool transparentBG = false;
		FogSettings fog;
		ShadowSettings shadows;
	};
	struct SceneData {
		SceneGraphicsData graphicsData;
		bool enablePhsyics3D = true;
		uint32_t shaderFlags = 0;
	};

	class STULU_API SceneCamera;
	class STULU_API MonoObjectInstance;
	class STULU_API SceneRenderer;
	class STULU_API EventCaller;

	class STULU_API Scene : public Registry {
	public:
		Scene();
		Scene(const SceneData data, bool createSceneCaller = true);
		~Scene();

		virtual GameObject Create(const std::string& name, entt::entity id = entt::null) override;
		virtual void Destroy(GameObject gameObject) override;

		virtual bool IsScene() const override { return true; }
		virtual Scene* GetAsScene() override { return this; }

		void onUpdateEditor(SceneCamera& camera, bool render = true);
		void onRuntimeStart();
		void onUpdateRuntime(bool render = true);
		void onRuntimeStop();

		void onViewportResize(uint32_t width, uint32_t height);
		void onEvent(Stulu::Event& e);

		GameObject getMainCamera();
		GameObject getMainLight();
		inline SceneData& getData() { return m_data; }
		inline Ref<EventCaller> getCaller() const { return m_caller; }

		bool PhysicsEnable() const { return m_data.enablePhsyics3D; }
		SceneRenderer* getRenderer() const { return m_renderer.get(); }
		int32_t getViewportWidth() const { return m_viewportWidth; }
		int32_t getViewportHeight() const { return m_viewportHeight; }
		float GetSceneRuntime() const { return m_sceneRuntimeTime; }

		void GeneralUpdates();

		static Ref<Scene> copy(Ref<Scene> scene);

		//sets the scene up, renderes for every cam, closes for renderering
		void renderScene(bool callEvents = true);
		void setupSceneForRendering(bool callEvents = true);
		//needs setup and closing
		void renderSceneForCamera(GameObject gameObject, bool callEvents = true);
		void closeSceneForRendering();
	private:
		uint32_t m_viewportWidth = 1, m_viewportHeight = 1;
		SceneData m_data;
		Scope<SceneRenderer> m_renderer = nullptr;
		Ref<EventCaller> m_caller = nullptr;
		float m_sceneRuntimeTime = 0.0f;
		bool m_updatesRan = false;

		void renderSceneEditor(SceneCamera& camera);
	};
}

