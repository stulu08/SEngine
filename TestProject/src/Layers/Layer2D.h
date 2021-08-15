#include <Stulu.h>



class Layer2D : public Stulu::Layer
{
public:
	Layer2D()
		:Layer("2D"), m_cameraController(1280.0f / 720.0f, true, true, true) {
	}

	void onAttach() override;
	void onUpdate(Stulu::Timestep timestep) override;
	void onEvent(Stulu::Event& e) override;
	void onImguiRender(Stulu::Timestep timestep) override;


private:

	Stulu::OrthographicCameraController m_cameraController;

	Stulu::Transform m_transformOne;
	Stulu::Transform m_transformTwo;
	glm::vec4 m_colorOne = COLOR_PLASTIC_CYAN_VEC4;
	glm::vec4 m_colorTwo = COLOR_PLASTIC_RED_VEC4;

	Stulu::Ref<Stulu::Texture2D> m_texture;
	glm::vec2 texTilling = glm::vec2(1.0f);


	glm::vec4 m_clearColor = glm::vec4(0.15f, 0.15f, 0.15f, 1.0f);
};

