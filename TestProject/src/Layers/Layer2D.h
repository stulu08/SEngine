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
	struct ProfileResult {
		const char* name;
		float time;
	};
	std::vector<ProfileResult> profileResults;

	Stulu::OrthographicCameraController m_cameraController;

	Stulu::Transform m_transform;
	glm::vec4 m_color = COLOR_PLASTIC_BLUE_VEC4;

	glm::vec4 m_clearColor = glm::vec4(0.15f, 0.15f, 0.15f, 1.0f);
};

