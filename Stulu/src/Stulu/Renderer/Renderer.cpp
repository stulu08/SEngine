#include "st_pch.h"
#include "Renderer.h"
#include "Platform/OpenGL/OpenGLShader.h"
#include "Stulu/Renderer/Renderer2D.h"

namespace Stulu {
	Scope<Renderer::SceneData> Renderer::m_sceneData = createScope<Renderer::SceneData>();

	void Renderer::init() {
		ST_PROFILING_FUNCTION();
		RenderCommand::init();
		Renderer2D::init();
		setUpSkyBox();
	}
	void Renderer::setUpSkyBox() {
		ST_PROFILING_FUNCTION();
		m_sceneData->skybox.shader = Shader::create("Stulu/assets/Shaders/SkyBox.glsl");
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
		ST_PROFILING_FUNCTION();
		RenderCommand::setViewport(0, 0, e.getWidth(), e.getHeight());
	}

	void Renderer::beginScene(Camera& cam, bool skybox) {
		ST_PROFILING_FUNCTION();
		m_sceneData->viewProjectionMatrix = cam.getViewProjectionMatrix();
		m_sceneData->skyBoxClear = skybox;
		m_sceneData->view = cam.getViewMatrix();
		m_sceneData->proj = cam.getProjectionMatrix();
		m_sceneData->camPos = cam.getPos();
	}
	void Renderer::endScene() {

		ST_PROFILING_FUNCTION();
		//draw Skybox
		if (!m_sceneData->skyBoxClear)
			return;
		RenderCommand::setDepthFunc(true);
		m_sceneData->skybox.shader->bind();
		glm::mat4 _view = glm::mat4(glm::mat3(m_sceneData->view));
		m_sceneData->skybox.shader->setMat4("u_viewProjection", m_sceneData->proj * _view);
		m_sceneData->skybox.shader->setMat4("u_transform", glm::mat4(1.0f));
		m_sceneData->skybox.vertexArray->bind();
		m_sceneData->skybox.texture->bind();
		RenderCommand::drawPrimitiveArray(m_sceneData->skybox.count);
		RenderCommand::setDepthFunc(false);
	}
	void Renderer::submit(const Ref<VertexArray>& vertexArray, const Ref<Shader>& shader, const glm::mat4& transform) {
		ST_PROFILING_FUNCTION();
		shader->bind();
		shader->setMat4("u_viewProjection", m_sceneData->viewProjectionMatrix);
		shader->setMat4("u_transform", transform);
		shader->setFloat3("u_camPos", m_sceneData->camPos);
		vertexArray->bind();
		
		RenderCommand::drawIndexed(vertexArray);
	}
	void Renderer::submit(Mesh& mesh, const Ref<Shader>& shader, const glm::mat4& transform) {
		submit(mesh.getVertexArray(), shader, transform);
	}
}