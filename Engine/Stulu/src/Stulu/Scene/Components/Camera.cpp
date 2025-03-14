#include "st_pch.h"
#include "Stulu/Renderer/Camera.h"
#include "Stulu/Scene/Components/Camera.h"

#include "Components.h"

namespace Stulu {
	CameraComponent::CameraComponent() 
		: m_cam(m_mode, FrameBufferSpecs(100, 100)) {
		UpdateCamera();

	}
	CameraComponent::CameraComponent(CameraMode mode, uint32_t width, uint32_t height)
		: m_cam(m_mode, FrameBufferSpecs(width, height)) {
		UpdateCamera();
	}

	CameraComponent::CameraComponent(const CameraComponent& other)
		: m_cam(other.m_cam), Component(other) {
		m_renderTarget = other.m_renderTarget;
		m_mode = other.m_mode;
		m_depth = other.m_depth;
		m_fov = other.m_fov;
		m_near = other.m_near;
		m_far = other.m_far;
		m_clearColor = other.m_clearColor;
		m_clearType = other.m_clearType;

		UpdateCamera();
	}

	CameraComponent::CameraComponent(const Camera& camera)
		: m_cam(camera), m_mode(camera.getMode()) {

	}


	void CameraComponent::ResizeTexture(uint32_t width, uint32_t height) {

		if (IsRenderTarget() && AssetsManager::existsAndType(m_renderTarget, AssetType::RenderTexture)) {
			Asset& asset = AssetsManager::get(m_renderTarget);
			GetTexture()->uuid = m_renderTarget;
			asset.data = std::dynamic_pointer_cast<Texture>(GetTexture());
			// resizeing
		}
		else {
			m_cam.ResizeFrameBuffer(width, height);
		}

	}

	void CameraComponent::UpdateCamera() {
		const float aspect = GetAspect();
		if (m_mode == CameraMode::Perspective) {
			m_cam.SetPrespective(m_fov, aspect, m_near, m_far);
		}
		else if (m_mode == CameraMode::Orthographic) {
			m_cam.SetOrthographic(-aspect * GetZoom(), aspect * GetZoom(), -GetZoom(), GetZoom(), m_near, m_far);
		}
		else {
			// idgaf
			m_mode = CameraMode::Perspective;
			UpdateCamera();
		}
	}
	Frustum CameraComponent::CalculateFrustum(TransformComponent& transform) const {
		const float aspect = GetAspect();

		if (m_mode == CameraMode::Perspective) {
			return VFC::createFrustum(aspect, m_near, m_far, m_fov, transform);
		}
		else {
			return VFC::createFrustum_ortho(-aspect * GetZoom(), aspect * GetZoom(), -GetZoom(), GetZoom(), m_near, m_far, transform);
		}
	}
}