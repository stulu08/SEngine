#include <Stulu.h>

class DefaultLayer : public Stulu::Layer
{
public:
	DefaultLayer()
		:Layer("Default"), m_camera(-1.6f,1.6f,-0.9f,0.9f) {
		app = &Stulu::Application::get();
	}

	void onAttach() override;
	void onUpdate() override;
	void onEvent(Stulu::Event& e) override;
	void drawRendererInfos();
	void drawCameraInfos();
	void onImguiRender() override;
	
private:
	Stulu::Application* app;

	//cam
	Stulu::OrthographicCamera m_camera;

	//rainbow triangle
	std::shared_ptr<Stulu::Shader> m_rainbowShader;
	std::shared_ptr<Stulu::VertexArray> m_vertexArray;
	//blue bg square
	std::shared_ptr<Stulu::Shader> m_blueShader;
	std::shared_ptr<Stulu::VertexArray> m_squareVertexArray;
};

