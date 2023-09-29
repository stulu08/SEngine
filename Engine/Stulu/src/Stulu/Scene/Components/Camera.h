#pragma once
#include "Stulu/Renderer/OrthographicCamera.h"
#include "Stulu/Renderer/PerspectiveCamera.h"
#include "Component.h"
namespace Stulu {
#define BLOOM_MAX_SAMPLES 10u
	struct PostProcessingData {
		float enableGammaCorrection = 1.0f;
		float gamma = 2.2f;
		float exposure = 1.0f;
		struct Bloom {
			Ref<Texture2D> downSample[BLOOM_MAX_SAMPLES];
			Ref<Texture2D> upSample[BLOOM_MAX_SAMPLES];
			uint32_t samples = 0;
			float bloomIntensity = .5f;
			float bloomTreshold = 1.1f;
			bool bloomEnabled = false;
		} bloomData;
	};
	class STULU_API Component;
	class STULU_API CameraComponent : public Component {
	public:
		CameraComponent() {
			cam = createRef<PerspectiveCamera>(settings.fov, settings.aspectRatio, settings.zNear, settings.zFar);
		}
		CameraComponent(const CameraComponent&) = default;
		CameraComponent(const CameraMode mode);

		void onResize(uint32_t width, uint32_t height);
		void updateSize();
		void updateProjection();
		void updateMode();

		const glm::mat4& getProjection() const { return cam->getProjectionMatrix(); }

		const Ref<Camera>& getNativeCamera() const { return cam; }
		const Ref<FrameBuffer>& getFrameBuffer() const { return cam->getFrameBuffer(); }
		const Ref<FrameBufferTexture>& getTexture() const { return cam->getFrameBuffer()->getTexture(); }
		const Frustum& getFrustum() const { return frustum; }

		enum class ClearType { Color = 0, Skybox = 1 };
		struct Settings {
			float fov = 80.0f, zoom = 1.0f, zNear = .01f, zFar = 250.0f;
			bool isRenderTarget = false;
			ClearType clearType = ClearType::Color;
			//if static aspect
			float aspectRatio = 0.0f;//readonly
			uint32_t textureWidth = 100, textureHeight = 100;
			//if clearType = Color
			glm::vec4 clearColor = glm::vec4(glm::vec3(.0f), 1.0f);
		} settings;
		CameraMode mode = CameraMode::Perspective;
		Frustum frustum;
		int depth = 1;
		UUID renderTexture = UUID::null;

		virtual void onComponentAdded(Scene* scene) override {
			if (scene->getViewportWidth() > 0 && scene->getViewportHeight() > 0)
				onResize(scene->getViewportWidth(), scene->getViewportHeight());
		}
	private:
		Ref<Camera> cam = nullptr;

		PostProcessingData* postProcessing = nullptr;
		friend class Scene;
		friend class SceneRenderer;
		friend class EditorLayer;
	};
}