#pragma once
#include "Editor/Panel/EditorHierarchy.h"
#include "Editor/Panel/SceneViewport.h"
#include "Editor/Panel/GameViewport.h"
#include "Editor/Panel/AssetBrowser.h"
#include "Editor/Panel/Profiling.h"
#include "Editor/Panel/StyleEditor.h"

namespace Stulu {
	class EditorLayer : public Layer {
	public:
		EditorLayer();
		~EditorLayer() {  }

		void onAttach() override;
		void onUpdate(Timestep timestep) override;
		void onImguiRender(Timestep timestep) override;
		void onRenderGizmo() override;
		void onEvent(Event& e) override;

		inline static Ref<Scene>& getActiveScene() { return m_activeScene; }

		void SaveScene(const std::string& path);
		void OpenScene(const std::string& path);
		inline static bool isRuntime() { return s_runtime; }

		void loadPanelConfig();
		void savePanelConfig();
	private:
		bool m_showStyleEditor = true;
		bool m_showHierarchy = true;
		bool m_showInspector = true;
		bool m_showAssetBrowser = true;
		bool m_showGameViewport = true;
		bool m_showSceneViewport = true;
		bool m_showProfiling = true;
		bool m_showSceneSettingsPanel = true;

		SceneCamera m_sceneCamera;
		std::string m_currentScenePath;
		GizmoTransformEditMode m_gizmoEditType = GizmoTransformEditMode::None;

		SceneViewportPanel m_sceneViewport;
		GameViewportPanel m_gameViewport;
		AssetBrowserPanel m_assetBrowser;
		EditorHierarchyPanel m_editorHierarchy;
		EditorInspectorPanel m_inspectorPanel;
		ProfilingPanel m_profilingPanel;

		struct FbDrawData {
			Ref<FrameBuffer> m_framebuffer;
			Ref<Shader> m_quadShader;
			Ref<VertexArray> m_quadVertexArray;
		} m_fbDrawData;


		void drawMenuBar();
		bool onShortCut(KeyDownEvent& e);
		void SaveScene();
		void OpenScene();
		void newScene();


		inline static Ref<Scene> m_activeScene = nullptr;
		inline static bool s_runtime = false;
	};
}