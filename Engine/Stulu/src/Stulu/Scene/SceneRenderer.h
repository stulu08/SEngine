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
			MeshRendererComponent* meshRenderer;
			MeshFilterComponent* meshFilter;
			TransformComponent* transform;
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
		void DrawSceneToCamera(TransformComponent& transform, CameraComponent& cam, bool callEvents = false);

		void Clear();
		void Clear(CameraComponent& cam);

		void RegisterObject(MeshRendererComponent& mesh, MeshFilterComponent& filter, TransformComponent& transform);
		void RegisterObject(RenderObject&& object);

		// combines all the textures of the cameras, sorting by layer index
		void GenSceneTexture(const Ref<FrameBuffer>& sceneFbo);
		void ApplyPostProcessing(SceneCamera& camera);
		void ApplyPostProcessing(const Ref<FrameBuffer>& frameBuffer, PostProcessingComponent* data);
		void ApplyPostProcessing(const Ref<FrameBuffer>& destination, const Texture2D* source, PostProcessingComponent* data);

		void drawSceneShadow();
		void drawScene();
		void drawSkyBox(TestMaterial* skybox);

		void resizeShadowMap();

		void Register3dObjects();

		void drawAll2d(const TransformComponent& camera, bool callEvents = false);

		const RenderStats& GetStats() const {
			return m_stats;
		}

		Ref<FrameBuffer> GetShadowMap() const {
			return m_shadowMap;
		}

		glm::mat4 GetLightSpaceMatrix(float nearPlane, float farPlane, const TransformComponent& cameraTransform, const CameraComponent& cameraComp, const TransformComponent& lightTransform) const;

	private:
		std::vector<RenderObject> m_drawList;
		std::vector<RenderObject> m_transparentDrawList;

		//shadows
		Ref<Shader> m_shadowShader;
		Ref<FrameBuffer> m_shadowMap;
		int32_t m_shadowCaster = -1;

		LightBufferData m_lightBufferData = LightBufferData();
		SceneBufferData m_sceneBufferData = SceneBufferData();

		Scene* m_scene;
		
		CpuDispatcher* m_dispatcher;
		uint32_t m_startedTasks = 0;
		std::atomic_uint32_t m_finishedTasks;

		RenderStats m_stats;

		friend class EditorLayer;
	};
}

