#include <Stulu.h>
#include "ParticleSystem.h"
class Layer2D : public Stulu::Layer
{
public:
	Layer2D()
		:Layer("2D"), m_cameraController(1280.0f / 720.0f, true,true,true), particleSystem(&particleSystemData){
	}

	void onAttach() override;
	void onUpdate(Stulu::Timestep timestep) override;
	void onEvent(Stulu::Event& e) override;
	void onImguiRender(Stulu::Timestep timestep) override;


private:
	int m_pSpawnCount = 2;
	ParticleSystemData particleSystemData;
	ParticleSystem particleSystem;

	Stulu::OrthographicCameraController m_cameraController;
	glm::vec3 m_particlePos = glm::vec3(.0f,-.5f,.1f);
	bool m_particleMouseControl = false;

	Stulu::Transform m_transformOne;
	Stulu::Transform m_transformTwo;
	Stulu::Transform m_transformThree;
	glm::vec4 m_colorOne = COLOR_PLASTIC_CYAN_VEC4;
	glm::vec4 m_colorTwo = COLOR_PLASTIC_RED_VEC4;
	glm::vec4 m_colorThree = COLOR_WHITE_VEC4;

	Stulu::Ref<Stulu::Texture2D> m_texture;
	glm::vec4 m_clearColor = glm::vec4(0.15f, 0.15f, 0.15f, 1.0f);
};

