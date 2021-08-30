#include <Stulu.h>

class Chunk {
public:
	Chunk();
	Chunk(glm::vec2 chunkCoords);
	void update(glm::vec3 player);

	Stulu::Ref<Stulu::VertexArray> vertexArray;
	Stulu::Transform transform;
	bool hidden;
	bool generated;

	glm::vec2 coords;

	static const uint32_t s_size;
	static const int s_viewDistance;
	static const int s_deleteDistance;
	static const float s_heightScale;
private:
	Stulu::BufferLayout layout = {
		{ Stulu::ShaderDataType::Float3, "a_pos" },
	};
};

class TerrainLayer : public Stulu::Layer
{
public:
	TerrainLayer()
		:Layer("Terrain"), m_cameraController(Stulu::Application::get().getWindow().getWidth() / Stulu::Application::get().getWindow().getHeight(), 85.0f, .1f, 300.0f) {
	}

	void onAttach() override;
	void onUpdate(Stulu::Timestep timestep) override;
	void onEvent(Stulu::Event& e) override;
	void onImguiRender(Stulu::Timestep timestep) override;
private:
	bool containsChunk(glm::vec2& Ck);
	void genChunks(glm::vec3& pos);

	Stulu::PerspectiveCameraController m_cameraController;
	glm::vec4 m_clearColor = glm::vec4(0.6f, 0.8f, 0.95f, 1.0f);

	std::vector<Chunk> chunks;

	Stulu::ShaderLibary m_shaderLib;
};

