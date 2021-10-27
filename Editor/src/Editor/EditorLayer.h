#pragma once
#include "Editor/Panel/EditorHierarchy.h"
#include "Editor/Panel/SceneViewport.h"
#include "Editor/Panel/GameViewport.h"
#include "Editor/Panel/AssetBrowser.h"

namespace Stulu {
	class EditorLayer : public Layer {
	public:
		EditorLayer();

		void onAttach() override;
		void onUpdate(Timestep timestep) override;
		void onImguiRender(Timestep timestep) override;
		void onEvent(Event& e) override;

		inline static Ref<Scene>& getActiveScene() { return m_activeScene; }

		void SaveScene(const std::string& path);
		void OpenScene(const std::string& path);
		inline static bool isRuntime() { return s_runtime; }
	private:
		bool m_showStyleEditor = false;
		bool m_showHierarchy = true;
		bool m_showInspector = true;
		bool m_showAssetBrowser = true;
		bool m_showGameViewport = true;
		bool m_showSceneViewport = true;
		
		std::string m_currentScenePath;

		SceneCamera m_sceneCamera;

		Ref<Material> m_material;
		ShaderLibary m_shaderLib;

		
		SceneViewportPanel m_sceneViewport;
		GameViewportPanel m_gameViewport;
		AssetBrowserPanel m_assetBrowser;
		EditorHierarchyPanel m_editorHierarchy;
		EditorInspectorPanel m_inspectorPanel;


		void drawMenuBar();
		bool onShortCut(KeyDownEvent& e);
		void SaveScene();
		void OpenScene();
		void newScene();


		inline static Ref<Scene> m_activeScene = nullptr;
		inline static bool s_runtime = false;
	};
}