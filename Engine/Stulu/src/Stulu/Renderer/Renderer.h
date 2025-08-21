#pragma once

#include "Stulu/Renderer/Mesh.h"
#include "Stulu/Renderer/RenderCommand.h"
#include "Stulu/Renderer/Camera.h"
#include "Stulu/Renderer/ShaderSystem.h"
#include "Stulu/Events/ApplicationEvent.h"
#include "Stulu/Renderer/Buffer.h"
#include "Stulu/Renderer/Material/TestMaterial.h"
#include "Shader/SceneBufferData.h"
#include "Shader/BufferData.h"

namespace Stulu{
	struct DrawCommand {
		uint32_t count;

		uint32_t meshIndex;
		Mesh* mesh;

		TestMaterial* material;
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
		static void Shutdown();
		static void onWindowResize(WindowResizeEvent& e);

		static void ScreenQuad(const Ref<FrameBuffer>& destination, const Shader* shader = nullptr, const glm::vec4& userData = glm::vec4(1.0f));
		static inline void ScreenQuad(const Ref<FrameBuffer>& destination, const Ref<Shader>& shader = nullptr, const glm::vec4& userData = glm::vec4(1.0f)) {
			ScreenQuad(destination, shader.get(), userData);
		}

		static void UploadModelData(const glm::mat4& transform, const glm::mat4& normalMatrix, uint32_t id = UINT32_MAX);
		static void UploadModelData(const SharedModelDataBuffer& buffer, size_t size);
		static inline void UploadModelData(const glm::mat4& transform) {
			UploadModelData(transform, glm::transpose(glm::inverse(transform)), UINT32_MAX);
		}

		static void RenderSkyBoxCube();

		static void BlibRenderBuffferToResultBuffer(
			const Ref<FrameBuffer>& renderBuffer, const Ref<FrameBuffer>& resultBuffer, 
			bool BlitColor = true, bool BlitDepth = true, bool BlitStencil = true);

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