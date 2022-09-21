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
			glm::mat4 normalMatrix;
			CullMode cullmode = CullMode::Back;
			Ref<BoundingBox> boundingBox = nullptr;
			TransformComponent* transformComp = nullptr;

			inline RenderObject& operator=(RenderObject& b) {
				material = b.material;//material
				vertexArray = b.vertexArray;//mesh
				transform = b.transform;//transform
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
		static void DrawSceneToCamera(TransformComponent& transform, CameraComponent& cam);

		static void Clear();
		static void Clear(CameraComponent& cam);

		static void RegisterObject(MeshRendererComponent& mesh, MeshFilterComponent& filter, TransformComponent& transform);
		static void RegisterObject(const Ref<VertexArray>& vertexArray, const Ref<Material>& material, const glm::mat4& transform);
		static void RegisterObject(const RenderObject& object);

		//combines all the textures of the cameras, sorting by layer index
		static void GenSceneTexture(const Ref<FrameBuffer>& sceneFbo, const Ref<Scene>& scene);

		static void ApplyPostProcessing(SceneCamera& camera);
		static void ApplyPostProcessing(CameraComponent& camera);
		static void ApplyPostProcessing(const Ref<FrameBuffer>& frameBuffer, PostProcessingData& data = PostProcessingData());
		static void ApplyPostProcessing(const Ref<FrameBuffer>& destination, const Ref<Texture>& source, PostProcessingData& data = PostProcessingData());

		//if useReflectionMap is false, a normal skybox will be used
		static void drawScene(const Ref<CubeMap>& reflectionMap);
		static void drawSkyBox(const Ref<CubeMap>& skybox);
	private:
		static void DoBloom(const Ref<FrameBuffer>& destination, const Ref<Texture>& source, PostProcessingData& data);

		struct RuntimeData {
			Ref<Shader> quadPostProcShader;
			std::vector<RenderObject> drawList;
			std::vector<RenderObject> transparentDrawList;

			LightBufferData lightBufferData;
			SceneBufferData sceneBufferData;
			PostProcessingBufferData postProcessingBufferData;

		};
		static RuntimeData s_data;

		friend class EditorLayer;
	};
}

