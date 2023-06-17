#pragma once
#include "Stulu/Scene/Components/Components.h"
#include "Stulu/Renderer/Renderer.h"
#include "SceneCamera.h"
namespace Stulu {
	struct SceneRendererCreateData {
		uint32_t shadowMapSize = 1024;
	};
	class STULU_API SceneRenderer {
	public:
		struct RenderObject {
			Ref<Material> material;
			Ref<VertexArray> vertexArray;
			glm::mat4 transform;
			glm::mat4 normalMatrix;
			CullMode cullmode = CullMode::Back;
			Ref<BoundingBox> boundingBox = nullptr;
			inline RenderObject& operator=(RenderObject& b) {
				material = b.material;//material
				vertexArray = b.vertexArray;//mesh
				transform = b.transform;//transform
				return *this;
			}
		};

		SceneRenderer(Scene* scene, uint32_t shadowMapSize = 1024);
		virtual ~SceneRenderer() {};

		void Begin();
		void End();

		void LoadLights();
		void AddLight(const TransformComponent& transform, const LightComponent& light);
		void UploadLightBuffer();

		void DrawSceneToCamera(SceneCamera& sceneCam, CameraComponent& mainCam);
		void DrawSceneToCamera(SceneCamera& sceneCam);
		void DrawSceneToCamera(TransformComponent& transform, CameraComponent& cam);

		void Clear();
		void Clear(CameraComponent& cam);

		void RegisterObject(MeshRendererComponent& mesh, MeshFilterComponent& filter, TransformComponent& transform);
		void RegisterObject(const Ref<VertexArray>& vertexArray, const Ref<Material>& material, const glm::mat4& transform, const Ref<BoundingBox>& transformedBoundingBox = nullptr);
		void RegisterObject(const RenderObject& object);

		//combines all the textures of the cameras, sorting by layer index
		void GenSceneTexture(const Ref<FrameBuffer>& sceneFbo);

		void ApplyPostProcessing(SceneCamera& camera);
		void ApplyPostProcessing(CameraComponent& camera);
		void ApplyPostProcessing(const Ref<FrameBuffer>& frameBuffer, PostProcessingData& data = PostProcessingData());
		void ApplyPostProcessing(const Ref<FrameBuffer>& destination, const Ref<Texture>& source, PostProcessingData& data = PostProcessingData());

		void drawSceneShadow();
		void drawScene();
		void drawSkyBox(const Ref<CubeMap>& skybox);

		void resizeShadowMap(uint32_t size);
	private:
		Ref<Texture> DoBloom(const Ref<FrameBuffer>& destination, const Ref<Texture>& source, PostProcessingData& data);

		static inline Ref<Shader> s_quadPostProcShader;

		std::vector<RenderObject> m_drawList;
		std::vector<RenderObject> m_transparentDrawList;

		//shadows
		static inline Ref<Shader> s_shadowShader;
		uint32_t m_shadowMapSize = 1024;
		Ref<FrameBuffer> m_shadowMap;
		int32_t m_shadowCaster = -1;

		//bloom
		const uint32_t m_minWidth = 7;
		const uint32_t m_minHeight = 5;
		const uint32_t m_maxSamples = BLOOM_MAX_SAMPLES;

		LightBufferData m_lightBufferData = LightBufferData();
		SceneBufferData m_sceneBufferData = SceneBufferData();
		PostProcessingBufferData m_postProcessingBufferData = PostProcessingBufferData();

		Scene* m_scene;

		friend class EditorLayer;
	};
}
