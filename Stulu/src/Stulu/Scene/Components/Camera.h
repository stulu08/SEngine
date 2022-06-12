#pragma once
#include "Stulu/Renderer/OrthographicCamera.h"
#include "Stulu/Renderer/PerspectiveCamera.h"
#include "Component.h"
namespace Stulu {
	class STULU_API Component;
	class STULU_API CameraComponent : public Component {
	public:
		CameraComponent() {
			cam = createRef<PerspectiveCamera>(settings.fov, settings.aspectRatio, settings.zNear, settings.zFar);
		}
		CameraComponent(const CameraComponent&) = default;
		CameraComponent(const CameraMode mode);

		const void onResize(uint32_t width, uint32_t height);
		const void updateSize();
		const void updateProjection();
		const void updateMode();

		const glm::mat4& getProjection() { return cam->getProjectionMatrix(); }

		const Ref<Camera>& getNativeCamera() { return cam; }
		const Ref<FrameBuffer>& getFrameBuffer() { return cam->getFrameBuffer(); }
		const Ref<FrameBufferTexture>& getTexture() { return cam->getFrameBuffer()->getTexture(); }

		enum ClearType { Color = 0, Skybox = 1 };
		struct Settings {
			float fov = 80.0f, zoom = 1.0f, zNear = .01f, zFar = 250.0f;
			bool staticAspect = false;
			ClearType clearType = Color;
			//if static aspect
			float aspectRatio = 0.0f;//readonly
			uint32_t textureWidth = 100, textureHeight = 100;
			//if cleaType = Color
			glm::vec4 clearColor = glm::vec4(glm::vec3(.0f), 1.0f);
		} settings;
		CameraMode mode = CameraMode::Perspective;
		int depth = -1;
	private:
		Ref<Camera> cam = nullptr;

		Ref<CubeMap> reflectionMap = nullptr;
		Ref<FrameBuffer> reflectionFrameBuffer = nullptr;

		friend class Scene;
		friend class SceneRenderer;
	};
}