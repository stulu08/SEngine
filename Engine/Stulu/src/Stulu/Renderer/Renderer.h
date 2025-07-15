#pragma once

#include "Stulu/Renderer/Mesh.h"
#include "Stulu/Renderer/RenderCommand.h"
#include "Stulu/Renderer/Camera.h"
#include "Stulu/Renderer/ShaderSystem.h"
#include "Stulu/Events/ApplicationEvent.h"
#include "Stulu/Renderer/Buffer.h"

namespace Stulu{
	struct alignas (16) FogSettings {
		glm::vec4 fogColor = glm::vec4(0.44f, 0.44f, 0.44f, 1.0f);
		float fogMode = 0.0f; /* 0 = disbaled, 1 = linear, 2 = exp, 3 = exp2  */
		float linearFogStart = 50.0f;
		float linearFogEnd = 250.0f;
		float fogDensity = 0.005f;
		float FogHorizonStrength = 0.0f;
		float FogHorizonFalloff = 15.0f;
		float EnableFogHorizon = 1.0f;
		float FogGroundStrength = 1.0f;
		float EnableGroundFog = 0.0f;
		float FogHorizonOffset = -1000.0f; 
		float FogHorizonHeightFalloff = 2000.0f; 
		float EnableHorizonHeightFog = 1.0f;
		enum class Mode {
			Disabled = 0,
			Linear = 1,
			Exp = 2,
			Exp2 = 3
		};
	};
	struct alignas (16) SceneBufferData {
		glm::mat4 skyBoxRotation = glm::mat4(1.0f);
		glm::vec4 clearColor = glm::vec4(.0f);

		// 4 scalars
		uint32_t useSkybox = 0;
		uint32_t shaderFlags = 0;
		int32_t shadowCaster = -1;
		uint32_t cascadeCount = 0;

		glm::mat4 lightSpaceMatrices[ST_MAX_SHADOW_CASCADES + 1] = {};
		glm::vec4 cascadePlaneDistances[ST_MAX_SHADOW_CASCADES + 1] = {};
		glm::vec4 cascadeBlendDistance = glm::vec4(1.0f);
		union {
			glm::vec4 shadowCasterPosPacked;
			struct {
				glm::vec3 shadowCasterPos;
				float env_lod;
			};
		};
		

		FogSettings fogSettings;
	};


	struct alignas (16) CameraBufferData {
		glm::mat4 viewProjectionMatrix;
		glm::mat4 viewMatrix;
		glm::mat4 projMatrix;
		glm::vec4 cameraPosition;
		glm::vec4 cameraRotation;
		glm::vec4 cameraNearFar;
	};
	struct alignas (16) LightBufferData {
		struct Light {
			glm::vec4 colorAndStrength = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
			glm::vec4 positionAndType = glm::vec4(1.0f, 5.0f, 10.0f, 0.0f);
			glm::vec4 rotation = glm::vec4(90.0f, .0f, .0f, 1.0f);
			glm::vec4 spotLightData = glm::vec4(1.0f);
		} lights[ST_MAXLIGHTS];
		uint32_t lightCount = 0;
	};
	struct alignas (16) GlobalMaterialBufferData {
		uint32_t alphaMode;
		float alphaCutOff;
	};

	struct alignas (16) StaticModelDataBuffer {
		// will reinterpreted as uint32_t, for performance gain float is used here, due to some weird x86 cpu instructions (test: +- 15fps), also std140 layout rules
		glm::vec4 entityID;
		glm::mat4 normal;
		glm::mat4 transform;
	};
	struct alignas (16) InstancedModelDataBuffer {
		glm::vec4 entityID;
		glm::mat4 instanceMatrices[ST_MAX_INSTANCES];

		inline void SetNormalMatrix(size_t index, const glm::mat4& norm) {
			instanceMatrices[index * 2] = norm;
		}
		inline void SetTranslationMatrix(size_t index, const glm::mat4& trans) {
			instanceMatrices[index * 2 + 1] = trans;
		}
		inline void SetNormalFromTranslation(size_t index, const glm::mat4& trans) {
			instanceMatrices[index * 2] = glm::transpose(glm::inverse(trans));
		}
	};
	struct alignas (16) SkinnedModelDataBuffer {
		glm::vec4 entityID;
		glm::mat4 boneMatrices[ST_MAX_BONES];
	};
	struct alignas (16) SharedModelDataBuffer {
		union {
			StaticModelDataBuffer staticData;
			InstancedModelDataBuffer instancedData;
			SkinnedModelDataBuffer skinnedData;
		};
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