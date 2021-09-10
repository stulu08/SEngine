#pragma once
#include "Temp/Skybox.h"
#include "Temp/ParticleSystem.h"
namespace Stulu {

	struct GameObject {
		bool enabled = true;
		std::string* name;
		Transform transform = Transform(glm::vec3(.0f), glm::vec3(.0f), glm::vec3(1.0f));
		std::vector<Component*> components;
	};

	class EditorLayer : public Layer {
	public:
		EditorLayer();

		void onAttach() override;
		void onUpdate(Timestep timestep) override;
		void onImguiRender(Timestep timestep) override;
		void onEvent(Event& e) override;
	private:
		float m_metalness = 1.0f, m_fov = 85.0f;
		uint32_t m_viewPortPanelWidth;
		uint32_t m_viewPortPanelHeight;

		Light* light = nullptr;

		Model car;
		Model cube;
		Model sphere;

		std::vector<GameObject*> objects;

		ShaderLibary shaderLib;

		Ref<FrameBuffer> m_frameBuffer;
		CameraController m_cameraController;
		Skybox m_skybox;
	};
}