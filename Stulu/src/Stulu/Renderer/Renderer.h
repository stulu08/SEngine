#pragma once

#include "Stulu/Renderer/Mesh.h"
#include "Stulu/Renderer/RenderCommand.h"
#include "Stulu/Renderer/Camera.h"
#include "Stulu/Renderer/Shader.h"
#include "Stulu/Events/ApplicationEvent.h"
#include "Stulu/Renderer/UniformBuffer.h"

namespace Stulu{
	enum class BufferBinding : uint32_t {
		Camera = 0,
		Model = 1,
		Light = 2,
		PostProcessing = 3,
		Scene = 4,
		Material = 5,
	};
	struct CameraBufferData {
		glm::mat4 viewProjectionMatrix;
		glm::mat4 viewMatrix;
		glm::mat4 projMatrix;
		glm::vec4 cameraPosition;
		glm::vec4 cameraRotation;
		glm::vec4 cameraNearFar;
	};
	struct LightBufferData {
		struct Light {
			glm::vec4 colorAndStrength = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
			glm::vec4 positionAndType = glm::vec4(1.0f, 5.0f, 10.0f, 0.0f);
			glm::vec4 rotation = glm::vec4(90.0f, .0f, .0f, 1.0f);
			glm::vec4 spotLightData = glm::vec4(1.0f);
		} lights[ST_MAXLIGHTS];
		uint32_t lightCount = 0;
	};
	struct PostProcessingBufferData {
		float time = 0.0f;
		float delta = 0.0f;
		float toneMappingExposure = 1.0f;
		float gamma = 2.2f;
	};
	struct SceneBufferData {
		glm::mat4 skyBoxRotation = glm::mat4(1.0f);
		glm::vec4 clearColor = glm::vec4(.0f);
		uint32_t useSkybox = 0;
		uint32_t skyboxMapType = 0;
		uint32_t shaderFlags = 0;
		float env_lod = 4.0f;
	};
	class STULU_API Renderer {
	public:
		
		static void init();
		static void onWindowResize(WindowResizeEvent& e);

		static void submit(const Ref<VertexArray>& vertexArray, const Ref<Shader>& shader, const glm::mat4& transform, uint32_t count = 0);
		static void submit(const Ref<VertexArray>& vertexArray, const Ref<Shader>& shader, const glm::mat4& transform, const glm::mat4& normalMatrix, uint32_t count = 0);

		static void uploadCameraBufferData(const CameraBufferData& data);
		static void uploadCameraBufferData(const glm::mat4& projection, const glm::mat4& transform, float z_near = 0, float z_far = 1000);
		static void uploadCameraBufferData(const glm::mat4& projection, const glm::mat4& view, const glm::vec3 position, const glm::vec3 rotation, float z_near = 0, float z_far = 1000);
		
		static void uploadBufferData(BufferBinding buffer, const void* data, uint32_t size, uint32_t offset = 0);

		static Ref<UniformBuffer>& getBuffer(BufferBinding buffer);
		//16kb
		static uint32_t getBufferMaxSize() { return 16000u; };

		inline static RenderAPI::API getRendererAPI() { return RenderAPI::getAPI(); }
	private:
		static struct Data {
			Ref<UniformBuffer> cameraDataUniformBuffer = nullptr;
			Ref<UniformBuffer> modelDataUniformBuffer = nullptr;
			Ref<UniformBuffer> lightDataUniformBuffer = nullptr;
			Ref<UniformBuffer> sceneDataUniformBuffer = nullptr;
			Ref<UniformBuffer> postProcessingDataUniformBuffer = nullptr;
			Ref<UniformBuffer> materialDataUniformBuffer = nullptr;
		}s_data;
	};

}