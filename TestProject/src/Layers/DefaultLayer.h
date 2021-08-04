#include <Stulu.h>

class DefaultLayer : public Stulu::Layer
{
public:
	DefaultLayer()
		:Layer("Default"), m_camera(-1.6f,1.6f,-.9f,.9f), m_gridPos(-.5f, -0.5f, .0f), m_squarePos(.0f, .0f, .0f) {
		app = &Stulu::Application::get();
	}

	void onAttach() override;
	void onUpdate(Stulu::Timestep timestep) override;
	void onEvent(Stulu::Event& e) override;
	void onImguiRender(Stulu::Timestep timestep) override;
	void drawRendererInfos();
	void drawCameraInfos();
	void drawApplicationInfos(Stulu::Timestep timestep);
	void drawGridMoveWindow();
	void drawSquareMoveWindow();

	
private:
	Stulu::Application* app;

	//cam
	Stulu::OrthographicCamera m_camera;

	//blue bg square
	Stulu::Ref<Stulu::Shader> m_colorShader;
	Stulu::Ref<Stulu::Shader> m_textureShader;
	Stulu::Ref<Stulu::VertexArray> m_squareVertexArray;
	Stulu::Ref<Stulu::Texture2D> m_texture;

	glm::vec3 m_camerpos = m_camera.getPosition();
	float m_camerarotation = m_camera.getRotation();

	glm::vec3 m_gridPos;
	glm::vec3 m_squarePos;

	glm::vec4 m_colorOne = glm::vec4(0.8, 0.2, 0.3, 1.0);
	glm::vec4 m_colorTwo = glm::vec4(0.2, 0.3, 0.8, 1.0);
	glm::vec4 m_clearColor = glm::vec4(0.15f, 0.15f, 0.15f, 1.0f);

};

