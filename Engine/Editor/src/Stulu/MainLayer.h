#pragma once
#include <Stulu.h>

using namespace Stulu;

namespace Editor {
	class MainLayer : public Layer {
	public:
		MainLayer();
		~MainLayer();

		void onAttach() override;
		void onUpdate(Timestep timestep) override;
		void onImguiRender(Timestep timestep) override;
		void onRenderGizmo() override;
		void onEvent(Event& e) override;

		Ref<Scene>& GetActiveScene() { return m_activeScene; }

		//void SaveScene(const std::string& path);
		//void OpenScene(const std::string& path);

		inline bool IsRuntime() { return m_runtime; }
		inline SceneCamera& GetCamera() { return m_sceneCamera; }
	private:
		Ref<FrameBuffer> m_sceneFrameBuffer;

		SceneCamera m_sceneCamera;
		PostProcessingData cameraPostProcessData;
		
		std::string m_currentScenePath;
		GizmoTransformEditMode m_gizmoEditType = GizmoTransformEditMode::None;

		//void drawObjectOutlines();
		//void onDrawGizmoSelected(GameObject gameObject);
		
		//void drawMenuBar();
		
		//bool onShortCut(KeyDownEvent& e);
		//bool onApplicationQuit(WindowCloseEvent& e);
		
		//void SaveScene();
		//void OpenScene();
		//void NewScene();
		
		//oid OnRuntimeStart();
		//oid OnRuntimeStop();

		Ref<Scene> m_activeScene = nullptr;
		Ref<Scene> m_editorScene = nullptr, m_runtimeScene = nullptr;

		bool m_runtime = false;
	};
}