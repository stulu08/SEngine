#include <Stulu.h>

class LightCube : public Stulu::Layer
{
	struct Cube {
		Stulu::Transform transform;
		Cube(Stulu::Transform trans = Stulu::Transform())
			:transform(trans){}
	};
public:
	LightCube()
		:Layer("Cube"), m_cameraController(1280.0f / 720.0f,65.0f,.1f,100.0f)  {
	}

	void onAttach() override;
	void onUpdate(Stulu::Timestep timestep) override;
	void onEvent(Stulu::Event& e) override;
	void onImguiRender(Stulu::Timestep timestep) override;

	void drawRendererInfos();
	void drawCameraInfos();
	void drawApplicationInfos(Stulu::Timestep timestep);

	
private:
	glm::vec3 lightPos = glm::vec3(5.0f);
	glm::vec3 lightColor = glm::vec3(1.0f, 1.0f, .9f);
	float lightStrength = .1f;
	Cube map[10 * 10];

	Stulu::PerspectiveCameraController m_cameraController;

	Stulu::ShaderLibary m_shaderLib;
	Stulu::Ref<Stulu::VertexArray> m_cubeVertexArray;
	Stulu::Ref<Stulu::Texture2D> m_lightIconTexture;
	Stulu::Transform m_cubeTransform;

	glm::vec4 m_clearColor = glm::vec4(0.15f, 0.15f, 0.15f, 1.0f);
};

