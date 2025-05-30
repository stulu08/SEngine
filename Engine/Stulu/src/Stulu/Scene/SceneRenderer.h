#pragma once
#include "Stulu/Scene/Components/Components.h"
#include "Stulu/Renderer/Renderer.h"
#include "SceneCamera.h"
namespace Stulu {
	struct SceneRendererCreateData {
		uint32_t shadowMapSize = 1024;
	};
	struct RenderStats {
		size_t drawCalls;
		size_t shadowDrawCalls;
	};
	class STULU_API SceneRenderer {
	public:
		struct RenderObject {
			TestMaterial* material;
			Ref<VertexArray> vertexArray;
			glm::mat4 transform;
			glm::mat4 normalMatrix;
			CullMode cullmode = CullMode::Back;
			BoundingBox* boundingBox = nullptr;
			inline RenderObject& operator=(RenderObject& b) {
				material = b.material;//material
				vertexArray = b.vertexArray;//mesh
				transform = b.transform;//transform
				return *this;
			}
		};

		SceneRenderer(Scene* scene);
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
		void RegisterObject(const Ref<VertexArray>& vertexArray, TestMaterial* material, const glm::mat4& transform, BoundingBox* transformedBoundingBox = nullptr);
		void RegisterObject(RenderObject&& object);

		//combines all the textures of the cameras, sorting by layer index
		void GenSceneTexture(const Ref<FrameBuffer>& sceneFbo);

		void ApplyPostProcessing(SceneCamera& camera);
		void ApplyPostProcessing(const Ref<FrameBuffer>& frameBuffer, PostProcessingData& data = PostProcessingData());
		void ApplyPostProcessing(const Ref<FrameBuffer>& destination, const Texture2D* source, PostProcessingData& data = PostProcessingData());

		void drawSceneShadow();
		void drawScene();
		void drawSkyBox(const CubeMap* skybox);

		void resizeShadowMap();

		void Register3dObjects();

		void drawAll2d(const TransformComponent& camera);

		const RenderStats& GetStats() const {
			return m_stats;
		}

		Ref<FrameBuffer> GetShadowMap() const {
			return m_shadowMap;
		}
	private:
		Texture* DoBloom(const Ref<FrameBuffer>& destination, const Texture2D* source, PostProcessingData& data);

		Ref<Shader> m_postProcShader;

		std::vector<RenderObject> m_drawList;
		std::vector<RenderObject> m_transparentDrawList;

		//shadows
		Ref<Shader> m_shadowShader;
		Ref<FrameBuffer> m_shadowMap;
		int32_t m_shadowCaster = -1;

		//bloom
		const uint32_t m_minWidth = 7;
		const uint32_t m_minHeight = 5;
		const uint32_t m_maxSamples = BLOOM_MAX_SAMPLES;
		Ref<Shader> m_upSampleShader;
		Ref<Shader> m_downSampleShader;
		Ref<Shader> m_filterShader;

		LightBufferData m_lightBufferData = LightBufferData();
		SceneBufferData m_sceneBufferData = SceneBufferData();
		PostProcessingBufferData m_postProcessingBufferData = PostProcessingBufferData();

		Scene* m_scene;
		
		CpuDispatcher* m_dispatcher;
		uint32_t m_startedTasks = 0;
		std::atomic_uint32_t m_finishedTasks;

		RenderStats m_stats;

		friend class EditorLayer;
	};
}

