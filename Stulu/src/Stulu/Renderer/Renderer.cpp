#include "st_pch.h"
#include "Renderer.h"
#include "Platform/OpenGL/OpenGLShader.h"
#include "Stulu/Renderer/Renderer2D.h"

namespace Stulu {
	Scope<Renderer::SceneData> Renderer::m_sceneData = createScope<Renderer::SceneData>();

	void Renderer::init() {
		RenderCommand::init();
		Renderer2D::init();

		m_sceneData->skybox.shader = Shader::create("assets/Shaders/SkyBox.glsl");
		m_sceneData->skybox.shader->bind();
		m_sceneData->skybox.shader->setInt("u_skyBox", 0);
		float skyboxVertices[6 * 3 * 6] = {
			// positions          
			-1.0f,  1.0f, -1.0f,
			-1.0f, -1.0f, -1.0f,
			 1.0f, -1.0f, -1.0f,
			 1.0f, -1.0f, -1.0f,
			 1.0f,  1.0f, -1.0f,
			-1.0f,  1.0f, -1.0f,

			-1.0f, -1.0f,  1.0f,
			-1.0f, -1.0f, -1.0f,
			-1.0f,  1.0f, -1.0f,
			-1.0f,  1.0f, -1.0f,
			-1.0f,  1.0f,  1.0f,
			-1.0f, -1.0f,  1.0f,

			 1.0f, -1.0f, -1.0f,
			 1.0f, -1.0f,  1.0f,
			 1.0f,  1.0f,  1.0f,
			 1.0f,  1.0f,  1.0f,
			 1.0f,  1.0f, -1.0f,
			 1.0f, -1.0f, -1.0f,

			-1.0f, -1.0f,  1.0f,
			-1.0f,  1.0f,  1.0f,
			 1.0f,  1.0f,  1.0f,
			 1.0f,  1.0f,  1.0f,
			 1.0f, -1.0f,  1.0f,
			-1.0f, -1.0f,  1.0f,

			-1.0f,  1.0f, -1.0f,
			 1.0f,  1.0f, -1.0f,
			 1.0f,  1.0f,  1.0f,
			 1.0f,  1.0f,  1.0f,
			-1.0f,  1.0f,  1.0f,
			-1.0f,  1.0f, -1.0f,

			-1.0f, -1.0f, -1.0f,
			-1.0f, -1.0f,  1.0f,
			 1.0f, -1.0f, -1.0f,
			 1.0f, -1.0f, -1.0f,
			-1.0f, -1.0f,  1.0f,
			 1.0f, -1.0f,  1.0f
		};
		m_sceneData->skybox.vertexArray = Stulu::VertexArray::create();
		Stulu::Ref<Stulu::VertexBuffer> vertexBuffer = Stulu::VertexBuffer::create(sizeof(skyboxVertices), skyboxVertices);
		Stulu::BufferLayout layout = {
				{ Stulu::ShaderDataType::Float3, "a_pos" },
		};
		vertexBuffer->setLayout(layout);
		m_sceneData->skybox.vertexArray->addVertexBuffer(vertexBuffer);
	}

	void Renderer::onWinndowResize(WindowResizeEvent& e)
	{
		RenderCommand::setViewport(0, 0, e.getWidth(), e.getHeight());
	}

	void Renderer::beginScene(Camera& cam, bool skybox) {
		m_sceneData->viewProjectionMatrix = cam.getViewProjectionMatrix();
		m_sceneData->skyBoxClear = skybox;
		m_sceneData->view = cam.getViewMatrix();
		m_sceneData->proj = cam.getProjectionMatrix();
	}
	void Renderer::endScene() {
		//draw Skybox
		if (!m_sceneData->skyBoxClear)
			return;
		RenderCommand::setDepthFunc(true);
		m_sceneData->skybox.shader->bind();
		glm::mat4 _view = glm::mat4(glm::mat3(m_sceneData->view));
		m_sceneData->skybox.shader->setMat4("u_view", _view);
		m_sceneData->skybox.shader->setMat4("u_projection", m_sceneData->proj);
		m_sceneData->skybox.shader->setMat4("u_transform", glm::mat4(1.0f));
		m_sceneData->skybox.vertexArray->bind();
		m_sceneData->skybox.texture->bind();
		RenderCommand::drawPrimitiveArray(m_sceneData->skybox.size);
		RenderCommand::setDepthFunc(false);
	}
	void Renderer::submit(const Ref<VertexArray>& vertexArray, const Ref<Shader>& shader, const glm::mat4& transform) {
		shader->bind();
		std::dynamic_pointer_cast<OpenGLShader>(shader)->uploadMat4Uniform("u_viewProjection", m_sceneData->viewProjectionMatrix);
		std::dynamic_pointer_cast<OpenGLShader>(shader)->uploadMat4Uniform("u_transform", transform);
		vertexArray->bind();
		if(m_sceneData->skyBoxClear)
			m_sceneData->skybox.texture->bind();
		RenderCommand::drawIndex(vertexArray);
	}
	void Renderer::submit(Mesh& mesh, const Ref<Shader>& shader, const glm::mat4& transform) {
		submit(mesh.getVertexArray(), shader, transform);
	}
}