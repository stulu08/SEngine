#pragma once
#include <Stulu.h>

class Skybox {
public:
	Skybox();
	Skybox(std::vector<std::string> faces);
	Skybox(Stulu::Ref<Stulu::CubeMap> texture);
	void load();
	void draw(const Stulu::Camera& cam);

	void setCubeMap(Stulu::Ref<Stulu::CubeMap> texture) {
		m_texture = texture;
	}


private:
	Stulu::Ref<Stulu::VertexArray> m_vertexArray;
	Stulu::Ref<Stulu::Shader> m_shader;
	Stulu::Ref<Stulu::CubeMap> m_texture;
	uint32_t m_indexCount = 36;
};