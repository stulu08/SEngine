#include <Stulu.h>

class DefaultLayer : public Stulu::Layer
{
public:
	DefaultLayer()
		:Layer("Default"), m_cameraController(1280.0f / 720.0f, true)  {
	}

	void onAttach() override;
	void onUpdate(Stulu::Timestep timestep) override;
	void onEvent(Stulu::Event& e) override;
	void onImguiRender(Stulu::Timestep timestep) override;

	void drawRendererInfos();
	void drawCameraInfos();
	void drawApplicationInfos(Stulu::Timestep timestep);

	
private:

	Stulu::OrthographicCameraController m_cameraController;

	Stulu::ShaderLibary m_shaderLib;
	Stulu::Ref<Stulu::VertexArray> m_cubeVertexArray;

	Stulu::Transform m_cubeTransform;

	glm::vec4 m_clearColor = glm::vec4(0.15f, 0.15f, 0.15f, 1.0f);
};

