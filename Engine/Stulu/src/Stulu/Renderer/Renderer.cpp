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
				s_data.postProcessingDataUniformBuffer = UniformBuffer::create(2048 /* should be enough */, (uint32_t)BufferBinding::PostProcessing);

			if (s_data.sceneDataUniformBuffer == nullptr)
				s_data.sceneDataUniformBuffer = UniformBuffer::create(sizeof(SceneBufferData), (uint32_t)BufferBinding::Scene);

			if (s_data.materialDataUniformBuffer == nullptr)
				s_data.materialDataUniformBuffer = UniformBuffer::create(getBufferMaxSize(), (uint32_t)BufferBinding::Material);

			if (s_data.modelDataUniformBuffer == nullptr)
				s_data.modelDataUniformBuffer = UniformBuffer::create(getBufferMaxSize(), (uint32_t)BufferBinding::Model);

			Renderer2D::init();
		}
	}
	void Renderer::Shutdown() {
		Renderer2D::shutdown();
		s_data = Data();
	}
	void Renderer::onWindowResize(WindowResizeEvent& e) {
		RenderCommand::setViewport(0, 0, e.getWidth(), e.getHeight());
	}

	void Renderer::ScreenQuad(const Ref<FrameBuffer>& destination, const Shader* shader, const glm::vec4& userData)  {
		if (!destination) {
			CORE_ASSERT(false, "Renderer::ScreenQuad no destination provided");
			return;
		}
		if (!shader) {
			CORE_ASSERT(false, "Renderer::ScreenQuad no shader provided");
			return;
		}
		

		shader->bind();

		struct Data{
			glm::vec4 userData = glm::vec4(0.0);
			float z = -1.0f;
			uint32_t pixelWidth = 1;
			uint32_t pixelHeight = 1;
		}data;
		data.pixelWidth = destination->getSpecs().width;
		data.pixelHeight = destination->getSpecs().height;
		data.userData = userData;

		Renderer::getBuffer(BufferBinding::Model)->setData(&data, sizeof(Data));
		RenderCommand::setCullMode(CullMode::Back);

		RenderCommand::setDepthTesting(false);
		destination->bind();
		RenderCommand::drawIndexed(Resources::getFullscreenVA(), 0);
		destination->unbind();
		RenderCommand::setDepthTesting(true);
	}

	void Renderer::UploadModelData(const glm::mat4& transform, const glm::mat4& normalMatrix, uint32_t id) {
		static StaticModelDataBuffer buffer;
		buffer.transform = transform;
		buffer.normal = normalMatrix;
		buffer.entityID = *reinterpret_cast<glm::vec4*>(&id);
		s_data.modelDataUniformBuffer->setData(&buffer, sizeof(buffer));
	}
	void Renderer::UploadModelData(const SharedModelDataBuffer& buffer, size_t size) {
		s_data.modelDataUniformBuffer->setData(&buffer, sizeof(buffer));
	}

	void Renderer::RenderSkyBoxCube() {
		RenderCommand::setCullMode(CullMode::BackAndFront);
		RenderCommand::drawIndexed(Resources::CubeMesh()->GetVertexArray());
	}

	void Renderer::BlibRenderBuffferToResultBuffer(
		const Ref<FrameBuffer>& renderBuffer, const Ref<FrameBuffer>& resultBuffer,
		bool BlibColor, bool BlibDepth, bool BlibStencil) {
		
		renderBuffer->BlitToOther(resultBuffer, BlibColor, BlibDepth, BlibStencil);
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