#pragma once
#include "Stulu/Scene/Components/Components.h"
#include "Stulu/Renderer/Renderer.h"
#include "SceneCamera.h"
namespace Stulu {
	class STULU_API SceneRenderer {
	public:
		struct RenderObject {
			Ref<Material> material;
			Ref<VertexArray> vertexArray;
			glm::mat4 transform;
			CullMode cullmode = CullMode::Back;
			Ref<BoundingBox> boundingBox = nullptr;
			TransformComponent* transformComp = nullptr;

			inline RenderObject& operator=(RenderObject& b) {
				material = b.material;
				vertexArray = b.vertexArray;
				transform = b.transform;
				cullmode = b.cullmode;
				return *this;
			}
		};

		static void init();

		static void Begin();
		static void End();

		static void LoadLights(Scene* scene);
		static void AddLight(const TransformComponent& transform, const LightComponent& light);
		static void UploadLightBuffer();

		static void DrawSceneToCamera(SceneCamera& sceneCam, CameraComponent& mainCam);
		static void DrawSceneToCamera(SceneCamera& sceneCam, Scene* scene);
		static void DrawSceneToCamera(TransformComponent& transform, CameraComponent& cam, bool drawSkyBox = true);

		static void Clear();
		static void Clear(CameraComponent& cam);

		static void RegisterObject(MeshRendererComponent& mesh, MeshFilterComponent& filter, TransformComponent& transform);
		static void RegisterObject(const Ref<VertexArray>& vertexArray, const Ref<Material>& material, const glm::mat4& transform, bool stencil = false);
		static void RegisterObject(const RenderObject& object);

		//combines all the textures of the cameras, sorting by layer index
		static void GenSceneTexture(const Ref<FrameBuffer>& sceneFbo, const Ref<Scene>& scene);

		//if useReflectionMap is false, a normal skybox will be used
		static void drawScene(const Ref<CubeMap>& reflectionMap);
		static void drawSkyBox(const Ref<CubeMap>& skybox);
	private:
		struct RuntimeData {
			Ref<UniformBuffer> lightBuffer = nullptr;
			Ref<UniformBuffer> postProcessungBuffer = nullptr;
			Ref<UniformBuffer> sceneDataBuffer = nullptr;

			struct FBDrawData{
				Ref<Shader> m_quadShader;
				Ref<VertexArray> m_quadVertexArray;
			} fbDrawData;
			std::vector<RenderObject> drawList;
			std::vector<RenderObject> transparentDrawList;
			std::vector<RenderObject> stencilDrawList;
		};
		static RuntimeData s_runtimeData;

		struct LightData {
			struct Light {
				glm::vec4 colorAndStrength = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
				glm::vec4 positionAndType = glm::vec4(1.0f, 5.0f, 10.0f, 0.0f);
				glm::vec4 rotation = glm::vec4(90.0f, .0f, .0f, 1.0f);
				glm::vec4 spotLightData = glm::vec4(1.0f);
			} lights[ST_MAXLIGHTS];
			uint32_t lightCount = 0;
		};
		struct PostProcessingData {
			float time = 0.0f;
			float delta = 0.0f;
		};
		struct ShaderSceneData {
			float toneMappingExposure = 1.0f;
			float gamma = 2.2f;
			float env_lod = 4.0f;
			uint32_t enableGammaCorrection = 1u;
			glm::vec4 clearColor = glm::vec4(.0f);
			uint32_t useSkybox = 0;
			uint32_t skyboxMapType = 0;
			uint32_t shaderFlags = 0;
		};

		static LightData s_lightData;
		static PostProcessingData s_postProcessingData;
		static ShaderSceneData s_shaderSceneData;

		friend class EditorLayer;
	};
}

