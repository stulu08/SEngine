#include <Stulu.h>

class DefaultLayer : public Stulu::Layer
{
public:
	DefaultLayer()
		:Layer("Default") {
		app = &Stulu::Application::get();
	}

	void onAttach() override;
	void onUpdate() override;
	void onEvent(Stulu::Event& e) override;

private:
	Stulu::Application* app;

	//rainbow triangle
	std::shared_ptr<Stulu::Shader> m_rainbowShader;
	std::shared_ptr<Stulu::VertexArray> m_vertexArray;
	//blue bg square
	std::shared_ptr<Stulu::Shader> m_blueShader;
	std::shared_ptr<Stulu::VertexArray> m_squareVertexArray;
};

