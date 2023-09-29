#pragma once
#include "Stulu/Renderer/PerspectiveCamera.h"
#include "Stulu/Core/Timestep.h"
#include "Stulu/Events/ApplicationEvent.h"
#include "Stulu/Scene/Components/Components.h"
#include "Stulu/Events/MouseEvent.h"
#include "Stulu/Events/KeyEvent.h"

namespace Stulu {
	class STULU_API SceneCamera {
	public:
		SceneCamera(float aspectRatio, float fov, float zNear, float zFar, uint32_t samples);

		void onUpdate(Timestep timestep);
		void updateMove(Timestep timestep);
		void onEvent(Event& e);
		void onResize(float width, float height);

		Ref<Camera>& getCamera() { return m_cam; }
		const Ref<Camera>& getCamera() const { return m_cam; }


		float getAspectRatio() const { return m_aspectRatio; }
		float getAspectRatioX() const { return m_aspectRatio; }
		float getAspectRatioY() const { return 1.0f; }
		glm::vec2 getAspectRatioXY() const { return { getAspectRatioX(),getAspectRatioY() }; }
		float getNear() const { return m_zNear; }
		float getFar() const { return m_zFar; }
		float getFov() const { return m_fov; }

		PostProcessingData& getPostProcessingData() { return postProcessingData; }
		bool getPostProcessingUsingMain() { return m_postProcessingUsingMain; }
		void setPostProcessingUsingMain(bool v) { m_postProcessingUsingMain = v; }
		const TransformComponent& getTransform() const { return m_transform; }
		TransformComponent& getTransform() { return m_transform; }
		const Frustum& getFrustum() const { return m_frustum; }
		Frustum& getFrustum() { return m_frustum; }
		void setTransform(const TransformComponent& transform) { m_transform = transform; }

	private:
		glm::vec2 m_mouseDelta = glm::vec2(0.0f);
		float m_aspectRatio, m_fov, m_zNear, m_zFar;
		float m_pitch = 0, m_yaw = 0;
		Ref<Camera> m_cam;
		TransformComponent m_transform;
		Frustum m_frustum;
		PostProcessingData postProcessingData;
		bool m_postProcessingUsingMain = true;

		float m_cameraMoveSpeed = 1.0f;
		float m_cameraSensitivity = .8f;


		void mouseTranslateMove(Timestep timestep);
		void mouseLookMove();

		bool onMouseScrolledEvent(MouseScrollEvent& e);

		friend class Scene;
		friend class SceneRenderer;
	};
}