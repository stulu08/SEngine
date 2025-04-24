#pragma once

#include "Stulu/Renderer/Mesh.h"
#include "Stulu/Renderer/RenderCommand.h"
#include "Stulu/Renderer/Camera.h"
#include "Stulu/Renderer/ShaderSystem.h"
#include "Stulu/Events/ApplicationEvent.h"
#include "Stulu/Renderer/Buffer.h"

namespace Stulu{
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
		float enableGammaCorrection = 1.0f;
		float toneMappingExposure = 1.0f;
		float gamma = 2.2f;
		float bloomStrength = 1.0f;
	};
	struct SceneBufferData {
		glm::mat4 skyBoxRotation = glm::mat4(1.0f);
		glm::vec4 clearColor = glm::vec4(.0f);
		uint32_t useSkybox = 0;
		uint32_t emptyData = 0;
		uint32_t shaderFlags = 0;
		float env_lod = 4.0f;
		glm::mat4 lightSpaceMatrix = glm::mat4(1.0f);
		glm::vec3 shadowCasterPos = glm::vec3(0.0);
		int32_t shadowCaster = -1;
	};
	struct GlobalMaterialBufferData {
		uint32_t alphaMode;
		float alphaCutOff;
	};
	class STULU_API Renderer {
	public:
		enum class API {
			none = 0,
			OpenGL = 1, Vulkan = 2,
			Direct3D11 = 3, Direct3D12 = 4,
			Metal = 5, GLES = 6
		};

		static void init();
		static void onWindowResize(WindowResizeEvent& e);

		static void ScreenQuad(const Ref<FrameBuffer>& destination, const Ref<Shader>& shader = nullptr);

		static void submit(const Ref<VertexArray>& vertexArray, const Ref<Shader>& shader, const glm::mat4& transform, uint32_t count = 0);
		static void submit(const Ref<VertexArray>& vertexArray, const Ref<Shader>& shader, const glm::mat4& transform, const glm::mat4& normalMatrix, uint32_t count = 0);

		static void uploadCameraBufferData(const CameraBufferData& data);
		static void uploadCameraBufferData(const glm::mat4& projection, const glm::mat4& view, const glm::vec3 position, const glm::vec3 rotation, float z_near = 0, float z_far = 1000);
		
		static void uploadBufferData(BufferBinding buffer, const void* data, uint32_t size, uint32_t offset = 0);

		static Ref<UniformBuffer>& getBuffer(BufferBinding buffer);
		static ShaderSystem* getShaderSystem() { return s_data.shaderSystem.get(); }
		//16kb
		static uint32_t getBufferMaxSize() { return 16000u; };

		inline static API getRendererAPI() { return s_data.api; }
	private:
		static struct Data {
			Ref<UniformBuffer> cameraDataUniformBuffer = nullptr;
			Ref<UniformBuffer> modelDataUniformBuffer = nullptr;
			Ref<UniformBuffer> lightDataUniformBuffer = nullptr;
			Ref<UniformBuffer> sceneDataUniformBuffer = nullptr;
			Ref<UniformBuffer> postProcessingDataUniformBuffer = nullptr;
			Ref<UniformBuffer> materialDataUniformBuffer = nullptr;
			Scope<ShaderSystem> shaderSystem = nullptr;
			API api;
		}s_data;

		friend class Application;
	};

}