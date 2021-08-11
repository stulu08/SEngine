#include <Stulu.h>

class ModelLayer : public Stulu::Layer
{
	//and material
	struct LightData {
		glm::vec3 lightColor = glm::vec3(1.0f, 1.0f, .9f);
		float lightStrength = 1.0f;
	};
public:
	ModelLayer()
		:Layer("ModelLayer"), m_cameraController(1280.0f / 720.0f, 65.0f, .1f, 100.0f) {
	}

	void onAttach() override;
	void onUpdate(Stulu::Timestep timestep) override;
	void onEvent(Stulu::Event& e) override;
	void onImguiRender(Stulu::Timestep timestep) override;

	void drawRendererInfos();
	void drawCameraInfos();
	void drawApplicationInfos(Stulu::Timestep timestep);


private:
	LightData lightData;

	Stulu::PerspectiveCameraController m_cameraController;

	Stulu::ShaderLibary m_shaderLib;
	Stulu::Ref<Stulu::Texture2D> m_lightIconTexture;

	

	Stulu::Model m_modelSphere;
	Stulu::Transform m_sphereTransform = Stulu::Transform(glm::vec3(2.0f),glm::vec3(.0f),glm::vec3(0.01f));

	Stulu::Model m_modelCar;
	Stulu::Transform m_carTransform = Stulu::Transform(glm::vec3(.0f), glm::vec3(.0f), glm::vec3(1.f));


	glm::vec4 m_clearColor = glm::vec4(0.15f, 0.15f, 0.15f, 1.0f);
};

