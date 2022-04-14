#pragma once
#include "Stulu/Scene/Components/Components.h"
#include "Stulu/Renderer/Renderer.h"
#include "SceneCamera.h"
namespace Stulu {
	class STULU_API SceneRenderer {
	public:
		struct RenderObject {
			Material* material;
			Ref<VertexArray> vertexArray;
			glm::mat4 transform;
			CullMode cullmode = CullMode::Back;
			float camDistance = .0f;

			inline RenderObject& operator=(RenderObject& b) {
				material = b.material;
				vertexArray = b.vertexArray;
				transform = b.transform;
				cullmode = b.cullmode;
				camDistance = b.camDistance;
				return *this;
			}
		};

		static void init();

		static void beginScene(GameObject object);
		static void beginScene(const SceneCamera& cam, GameObject mainCam);
		static void endScene();

		static void calculateLights(entt::basic_view<entt::entity, entt::exclude_t<>, TransformComponent, LightComponent> view);
		static void uploadBuffers(const SceneData& data);

		static void submit(MeshRendererComponent& mesh, MeshFilterComponent& filter, TransformComponent& transform);
		static void submit(const Ref<VertexArray>& vertexArray, Material* material, const glm::mat4& transform);
		static void submit(const RenderObject& object);

		static void flush();
		static void drawScene();
		static void drawSkyBox();
	private:
		static struct RuntimeData {
			struct LightData {
				struct Light {
					glm::vec4 colorAndStrength = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
					glm::vec4 positionAndType = glm::vec4(1.0f, 5.0f, 10.0f, 0.0f);
					glm::vec4 rotation = glm::vec4(90.0f, .0f, .0f, 1.0f);
					glm::vec4 spotLightData = glm::vec4(1.0f);
				} lights[MAXLIGHTS];
				uint32_t lightCount = 0;
			} lightData;
			struct SceneData {
				float toneMappingExposure = 1.0f;
				float gamma = 2.2f;
				float env_lod = 4.0f;
				uint32_t useSkybox = 0;
			} bufferData;
			inline static struct RenderObjectSkyBox{
				Ref<CubeMap> texture = nullptr;
				uint32_t mapType = 0;
			} camSkyBox;
			inline static Ref<Camera> cam = nullptr;
			inline static Ref<UniformBuffer> sceneDataBuffer = nullptr;
			inline static Ref<UniformBuffer> lightBuffer = nullptr;
			inline static glm::vec3 camPos;

		} s_runtimeData;

		inline static std::vector<RenderObject> s_drawList;
		//distance
		inline static std::vector<RenderObject> s_transparentDrawList;
		inline static std::vector<RenderObject> s_stencilDrawList;
	};
}

