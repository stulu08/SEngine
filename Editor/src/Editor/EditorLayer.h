#pragma once
#include "Temp/Skybox.h"
#include "Temp/ParticleSystem.h"
#include "Panel/EditorHierarchy.h"

namespace Stulu {
	class EditorLayer : public Layer {
	public:
		EditorLayer();

		void onAttach() override;
		void onUpdate(Timestep timestep) override;
		void onImguiRender(Timestep timestep) override;
		void onEvent(Event& e) override;
	private:
		bool m_viewPortFocused = false, m_viewPortHovered = false, m_runtime = false;


		uint32_t m_viewPortPanelWidth;
		uint32_t m_viewPortPanelHeight;

		Model cube;
		Model sphere;


		ShaderLibary shaderLib;

		Ref<FrameBuffer> m_frameBuffer;
		SceneCamera m_sceneCamera;
		Skybox m_skybox;

		Ref<Scene> m_activeScene;

		EditorHierarchyPanel m_editorHierarchy;

		bool onShortCut(KeyDownEvent& e);
	};
}