#pragma once
#include "Temp/Skybox.h"
#include "Temp/ParticleSystem.h"
#include "Panel/EditorHierarchy.h"
#include "Panel/EditorViewport.h"
namespace Stulu {
	class EditorLayer : public Layer {
	public:
		EditorLayer();

		void onAttach() override;
		void onUpdate(Timestep timestep) override;
		void onImguiRender(Timestep timestep) override;
		void onEvent(Event& e) override;
	private:
		bool m_runtime = false;
		std::string m_currentScenePath;

		SceneCamera m_sceneCamera;
		Ref<Scene> m_activeScene;

		ShaderLibary m_shaderLib;

		int m_gizmoEditType = -1;
		
		EditorViewportPanel m_sceneViewport;
		EditorViewportPanel m_gameViewport;

		EditorHierarchyPanel m_editorHierarchy;

		bool onShortCut(KeyDownEvent& e);
		void SaveScene();
		void OpenScene();
		void SaveScene(const std::string& path);
		void OpenScene(const std::string& path);
	};
}