#pragma once

#include "Stulu/Renderer/Mesh.h"
#include "Stulu/Renderer/RenderCommand.h"
#include "Stulu/Renderer/Camera.h"
#include "Stulu/Renderer/Shader.h"
#include "Stulu/Events/ApplicationEvent.h"
#include "Stulu/Renderer/UniformBuffer.h"

namespace Stulu{

	class STULU_API Renderer {
	public:
		struct MatricesBufferData {
			glm::mat4 viewProjectionMatrix;
			glm::mat4 viewMatrix;
			glm::mat4 projMatrix;
			glm::vec4 cameraPosition;
			glm::vec4 cameraRotation;
			glm::vec4 cameraNearFar;
			glm::mat4 modelMatrix;
		};
		static void init();
		static void onWindowResize(WindowResizeEvent& e);

		static void begin(const glm::mat4& projection, const glm::mat4& view = glm::mat4(1.0f), float z_near = 0, float z_far = 1000);
		static void begin(const glm::mat4& projection, const glm::mat4& view, const glm::vec3& position, const glm::vec3& rotation, float z_near = 0, float z_far = 1000);
		static void end();

		static void submit(const Ref<VertexArray>& vertexArray, const Ref<Shader>& shader = nullptr, const glm::mat4& transform = glm::mat4(1.0f), uint32_t count = 0);

		static void uploadBufferData(const glm::mat4& projection, const glm::mat4& transform, float z_near = 0, float z_far = 1000);
		static void uploadBufferData(const glm::mat4& projection, const glm::mat4& view, const glm::vec3 position, const glm::vec3 rotation, float z_near = 0, float z_far = 1000);
		//to upload data for custom shaders
		static void uploadBufferData(void* data, uint32_t size, uint32_t offset = 0);
		//16kb
		static uint32_t getBufferMaxSize() { return 16000u; };

		inline static RenderAPI::API getRendererAPI() { return RenderAPI::getAPI(); }
	private:
		static struct Data {
			MatricesBufferData matricesData;
			Ref<UniformBuffer> matricesDataUniformBuffer = nullptr;
		}m_runtimeData;
	};

}