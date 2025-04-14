#include "st_pch.h"
#include "Renderer.h"
#include "Platform/OpenGL/OpenGLShader.h"
#include "Stulu/Renderer/Renderer2D.h"
#include "Stulu/Resources/Resources.h"
#include "Stulu/Core/Application.h"

namespace Stulu {

	Renderer::Data Renderer::s_data;

	void Renderer::init() {
		s_data.shaderSystem = createScope<ShaderSystem>();
		// .../ShaderCache/{API-ID}/...
		std::string shaderCache = Application::get().getApplicationInfo().AppCachePath + "/ShaderCache/API-" + std::to_string((int)s_data.api) + "/";
		s_data.shaderSystem->SetCacheFolder(shaderCache);
		s_data.shaderSystem->LoadAllShaders(Resources::EngineDataDir + "/Stulu/Shader");

		if (getRendererAPI() == Renderer::API::OpenGL) {
			if (s_data.cameraDataUniformBuffer == nullptr)
				s_data.cameraDataUniformBuffer = UniformBuffer::create(sizeof(CameraBufferData), (uint32_t)BufferBinding::Camera);

			if (s_data.lightDataUniformBuffer == nullptr)
				s_data.lightDataUniformBuffer = UniformBuffer::create(sizeof(LightBufferData), (uint32_t)BufferBinding::Light);

			if (s_data.postProcessingDataUniformBuffer == nullptr)
				s_data.postProcessingDataUniformBuffer = UniformBuffer::create(sizeof(PostProcessingBufferData), (uint32_t)BufferBinding::PostProcessing);

			if (s_data.sceneDataUniformBuffer == nullptr)
				s_data.sceneDataUniformBuffer = UniformBuffer::create(sizeof(SceneBufferData), (uint32_t)BufferBinding::Scene);

			if (s_data.materialDataUniformBuffer == nullptr)
				s_data.materialDataUniformBuffer = UniformBuffer::create(getBufferMaxSize(), (uint32_t)BufferBinding::Material);

			if (s_data.modelDataUniformBuffer == nullptr)
				s_data.modelDataUniformBuffer = UniformBuffer::create(getBufferMaxSize(), (uint32_t)BufferBinding::Model);

			Renderer2D::init();
		}
	}
	void Renderer::onWindowResize(WindowResizeEvent& e) {
		RenderCommand::setViewport(0, 0, e.getWidth(), e.getHeight());
	}

	void Renderer::ScreenQuad(const Ref<FrameBuffer>& destination, const Ref<Shader>& shader)  {
		if (!destination) {
			CORE_ASSERT(false, "Renderer::ScreenQuad no destination provided");
			return;
		}
		if (!shader) {
			CORE_ASSERT(false, "Renderer::ScreenQuad no shader provided");
			return;
		}


		shader->bind();

		float z = -1.0f;
		Renderer::getBuffer(BufferBinding::Model)->setData(&z, sizeof(float));

		destination->bind();
		RenderCommand::drawIndexed(Resources::getFullscreenVA(), 0);
		destination->unbind();
	}

	void Renderer::submit(const Ref<VertexArray>& vertexArray, const Ref<Shader>& shader, const glm::mat4& transform, uint32_t count) {
		if (shader)
			shader->bind();

		static struct data { glm::mat4 normal; glm::mat4 transform; } data;
		data.transform = transform;
		data.normal = glm::transpose(glm::inverse(transform));
		s_data.modelDataUniformBuffer->setData(&data, sizeof(data));

		RenderCommand::drawIndexed(vertexArray, count);
	}
	void Renderer::submit(const Ref<VertexArray>& vertexArray, const Ref<Shader>& shader, const glm::mat4& transform, const glm::mat4& normalMatrix, uint32_t count) {
		if (shader)
			shader->bind();

		static struct data { glm::mat4 normal; glm::mat4 transform; } data;
		data.transform = transform;
		data.normal = normalMatrix;
		s_data.modelDataUniformBuffer->setData(&data, sizeof(data));

		RenderCommand::drawIndexed(vertexArray, count);
	}

	void Renderer::uploadCameraBufferData(const CameraBufferData& data) {
		uploadBufferData(BufferBinding::Camera, &data, sizeof(data), 0);
	}
	void Renderer::uploadCameraBufferData(const glm::mat4& projection, const glm::mat4& view, const glm::vec3 position, const glm::vec3 rotation, float z_near, float z_far) {
		static CameraBufferData cameraData;
		cameraData.viewMatrix = view;
		cameraData.projMatrix = projection;
		cameraData.viewProjectionMatrix = cameraData.projMatrix * cameraData.viewMatrix;
		cameraData.cameraPosition = glm::vec4(position,1.0f);
		cameraData.cameraRotation = glm::vec4(rotation, 1.0f);
		cameraData.cameraNearFar = glm::vec4(z_near, z_far, 0, 0);
		uploadBufferData(BufferBinding::Camera, &cameraData, sizeof(cameraData), 0);

	}
	
	void Renderer::uploadBufferData(BufferBinding binding, const void* data, uint32_t size, uint32_t offset) {
		getBuffer(binding)->setData(data, size, offset);
	}
	Ref<UniformBuffer>& Renderer::getBuffer(BufferBinding buffer) {
		switch (buffer)
		{
		case Stulu::BufferBinding::Camera:
			return s_data.cameraDataUniformBuffer;
		case Stulu::BufferBinding::Model:
			return s_data.modelDataUniformBuffer;;
		case Stulu::BufferBinding::Light:
			return s_data.lightDataUniformBuffer;;
		case Stulu::BufferBinding::PostProcessing:
			return s_data.postProcessingDataUniformBuffer;;
		case Stulu::BufferBinding::Scene:
			return s_data.sceneDataUniformBuffer;;
		case Stulu::BufferBinding::Material:
			return s_data.materialDataUniformBuffer;;
		}
		return s_data.modelDataUniformBuffer;
	}
}