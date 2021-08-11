#include "st_pch.h"
#include "Renderer2D.h"
#include "Stulu/Renderer/RenderCommand.h"
#include "Stulu/Renderer/Shader.h"

#include <glm/gtc/matrix_transform.hpp>

namespace Stulu {
	struct Renderer2DStorage {
		Ref<Shader> m_shader;
		Ref<Texture2D> m_texture;
		Ref<VertexArray> m_quadVertexArray;
		Ref<VertexArray> m_triangleVertexArray;
	};
	static Scope<Renderer2DStorage> s_renderer2Ddata;
	void Renderer2D::init() {
		s_renderer2Ddata = createScope<Renderer2DStorage>();
		Ref<VertexBuffer> vertexBuffer;
		Ref<IndexBuffer> indexBuffer;
		BufferLayout layout = {
			{ ShaderDataType::Float3, "a_pos" },
			{ ShaderDataType::Float3, "a_normals" },
			{ ShaderDataType::Float2, "a_texCoord" },
		};

		//quad
		uint32_t quadIndicies[6]{
			0,1,2,
			2,3,0
		};
		float quadVertices[8 * 4] = {
			-0.5f, -0.5f, .0f, -0.5f, -0.5f, .0f, 0, 0,
			 0.5f, -0.5f, .0f,  0.5f, -0.5f, .0f, 1, 0,
			 0.5f,  0.5f, .0f,  0.5f,  0.5f, .0f, 1, 1,
			-0.5f,  0.5f, .0f, -0.5f,  0.5f, .0f, 0, 1
		};
		s_renderer2Ddata->m_quadVertexArray = VertexArray::create();
		vertexBuffer = VertexBuffer::create(sizeof(quadVertices), quadVertices);
		vertexBuffer->setLayout(layout);
		s_renderer2Ddata->m_quadVertexArray->addVertexBuffer(vertexBuffer);
		indexBuffer = IndexBuffer::create(sizeof(quadIndicies) / sizeof(uint32_t), quadIndicies);
		s_renderer2Ddata->m_quadVertexArray->setIndexBuffer(indexBuffer);

		uint32_t triangleIndicies[3]{
			0,1,2,
		};
		float triangleVertices[8 * 5] = {
			-0.5f, -0.5f, .0f, -0.5f, -0.5f, .0f, 0, 0,
			 0.5f, -0.5f, .0f,  0.5f, -0.5f, .0f, 1, 0,
			  .0f,  0.5f, .0f,  .0f,   0.5f, .0f, 0.5, 1,
			//for texture coords
			 0.5f,  0.5f, .0f, 0.5f,  0.5f, .0f, 1, 1,
			-0.5f,  0.5f, .0f,-0.5f,  0.5f, .0f, 0, 1
		};
		s_renderer2Ddata->m_triangleVertexArray = VertexArray::create();
		vertexBuffer = VertexBuffer::create(sizeof(triangleVertices), triangleVertices);
		vertexBuffer->setLayout(layout);
		s_renderer2Ddata->m_triangleVertexArray->addVertexBuffer(vertexBuffer);
		indexBuffer = IndexBuffer::create(sizeof(triangleIndicies) / sizeof(uint32_t), triangleIndicies);
		s_renderer2Ddata->m_triangleVertexArray->setIndexBuffer(indexBuffer);


		s_renderer2Ddata->m_texture = Texture2D::create("assets/quad.png");
		s_renderer2Ddata->m_shader = Shader::create("assets/Shaders/default2D.glsl");
		s_renderer2Ddata->m_shader->bind();
		s_renderer2Ddata->m_shader->setInt("u_texture", 0);

	}
	void Renderer2D::shutdown() {
		delete s_renderer2Ddata.get();
	}
	void Renderer2D::beginScene(const Camera& cam) {
		s_renderer2Ddata->m_shader->bind();
		s_renderer2Ddata->m_shader->setMat4("u_viewProjection", cam.getViewProjectionMatrix());
	}
	void Renderer2D::endScene() {
	}
	void Renderer2D::drawQuad(const glm::vec2& pos, const glm::vec2& size,  const glm::vec3& rotation, const glm::vec4& color) {
		drawQuad(Transform(pos, rotation, size), color);
	}
	void Renderer2D::drawQuad(const glm::vec3& pos, const glm::vec2& size, const glm::vec3& rotation, const glm::vec4& color) {
		drawQuad(Transform(pos,rotation,glm::vec3(size.x,size.y,.0f)), color);
	}
	void Renderer2D::drawQuad(Transform& transform, const glm::vec4& color) {
		drawQuad(transform.toMat4(), color);
	}
	void Renderer2D::drawTriangle(const glm::vec2& pos, const glm::vec2& size, const glm::vec3& rotation, const glm::vec4& color) {
		drawTriangle(Transform(pos, rotation, size), color);
	}
	void Renderer2D::drawTriangle(const glm::vec3& pos, const glm::vec2& size, const glm::vec3& rotation, const glm::vec4& color) {
		drawTriangle(Transform(pos, rotation, glm::vec3(size.x, size.y, .0f)), color);
	}
	void Renderer2D::drawTriangle(Transform& transform, const glm::vec4& color) {
		drawTriangle(transform.toMat4(), color);
	}
	void Renderer2D::drawTexture2DQuad(const Stulu::Ref<Stulu::Texture2D> texture, const glm::vec2& pos, const glm::vec2& size, const glm::vec3& rotation, const glm::vec4& color) {
		drawTexture2DQuad(texture, Transform(pos, rotation, size), color);
	}
	void Renderer2D::drawTexture2DQuad(const Stulu::Ref<Stulu::Texture2D> texture, const glm::vec3& pos, const glm::vec2& size, const glm::vec3& rotation, const glm::vec4& color) {
		drawTexture2DQuad(texture, Transform(pos, rotation, size), color);
	}
	void Renderer2D::drawTexture2DQuad(const Stulu::Ref<Stulu::Texture2D> texture, Transform& transform, const glm::vec4& color) {
		drawTexture2DQuad(texture, transform.toMat4(), color);
	}

	void Renderer2D::drawQuad(const glm::mat4& transform, const glm::vec4& color) {

		s_renderer2Ddata->m_shader->setMat4("u_transform", transform);
		s_renderer2Ddata->m_shader->setFloat4("u_color", color);
		s_renderer2Ddata->m_texture->bind();

		s_renderer2Ddata->m_quadVertexArray->bind();
		RenderCommand::drawIndex(s_renderer2Ddata->m_quadVertexArray);
	}
	void Renderer2D::drawTriangle(const glm::mat4& transform, const glm::vec4& color) {

		s_renderer2Ddata->m_shader->setMat4("u_transform", transform);
		s_renderer2Ddata->m_shader->setFloat4("u_color", color);
		s_renderer2Ddata->m_texture->bind();

		s_renderer2Ddata->m_triangleVertexArray->bind();
		RenderCommand::drawIndex(s_renderer2Ddata->m_triangleVertexArray);
	}

	void Renderer2D::drawTexture2DQuad(const Stulu::Ref<Stulu::Texture2D> texture, const glm::mat4& transform, const glm::vec4& color) {

		s_renderer2Ddata->m_shader->setMat4("u_transform", transform);
		s_renderer2Ddata->m_shader->setFloat2("u_textureTiling", texture->tiling);
		s_renderer2Ddata->m_shader->setFloat4("u_color", color);
		texture->bind();

		s_renderer2Ddata->m_quadVertexArray->bind();
		RenderCommand::drawIndex(s_renderer2Ddata->m_quadVertexArray);
	}

}
