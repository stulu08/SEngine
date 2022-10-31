#include "st_pch.h"
#include "Camera.h"

#include "Components.h"
#include "Stulu/Core/Resources.h"

namespace Stulu {
	CameraComponent::CameraComponent(const CameraMode mode, bool genMap)
		: mode(mode) {
		if (mode == CameraMode::Perspective) {
			cam = createRef<PerspectiveCamera>(settings.fov, settings.aspectRatio, settings.zNear, settings.zFar);
		}
		else if (mode == CameraMode::Orthographic) {
			cam = createRef<OrthographicCamera>(-settings.aspectRatio * settings.zoom, settings.aspectRatio * settings.zoom, -settings.zoom, settings.zoom, settings.zNear, settings.zFar);
		}
			
		if (genMap) {
			if (reflectionMap == nullptr) {

				reflectionMap = CubeMap::create(ST_REFLECTION_MAP_SIZE, TextureSettings(TextureFormat::RGB16F));
			}
			if (reflectionFrameBuffer == nullptr) {
				FrameBufferSpecs specs;
				specs.width = ST_REFLECTION_MAP_SIZE;
				specs.height = ST_REFLECTION_MAP_SIZE;
				specs.colorTexture.format = TextureFormat::None;
				reflectionFrameBuffer = FrameBuffer::create(specs);
			}
		}

	}
	void CameraComponent::onResize(uint32_t width, uint32_t height) {
		ST_PROFILING_FUNCTION();
		settings.textureWidth = width;
		settings.textureHeight = height;
		updateSize();
	}
	void CameraComponent::updateSize() {
		ST_PROFILING_FUNCTION();
		settings.aspectRatio = (float)settings.textureWidth / (float)settings.textureHeight;
		cam->getFrameBuffer()->resize(settings.textureWidth, settings.textureHeight);
		updateProjection();
	}
	void CameraComponent::updateProjection() {
		ST_PROFILING_FUNCTION();
		if (mode == CameraMode::Perspective) {
			cam->setProjection(settings.fov, settings.aspectRatio, settings.zNear, settings.zFar);
			frustum = VFC::createFrustum(settings.aspectRatio, settings.zNear, settings.zFar, settings.fov, gameObject.getComponent<TransformComponent>());
		}
		else if (mode == CameraMode::Orthographic) {
			cam->setProjection(-settings.aspectRatio * settings.zoom, settings.aspectRatio * settings.zoom, -settings.zoom, settings.zoom, settings.zNear, settings.zFar);
			frustum = VFC::createFrustum(settings.aspectRatio, settings.zNear, settings.zFar, 0, gameObject.getComponent<TransformComponent>());
		}
		if (AssetsManager::existsAndType(renderTexture, AssetType::RenderTexture)) {
			Asset& asset = AssetsManager::get(renderTexture);
			if (!settings.isRenderTarget) {
				asset.data = Resources::getBlackTexture();
			}
			else {
				asset.data = std::dynamic_pointer_cast<Texture>(getTexture());
				std::any_cast<Ref<Texture>>(asset.data)->uuid = renderTexture;
			}
		}
		
	}
	void CameraComponent::updateMode() {
		ST_PROFILING_FUNCTION();
		if (mode == CameraMode::Orthographic) {
			cam.reset(new OrthographicCamera(-settings.aspectRatio * settings.zoom, settings.aspectRatio * settings.zoom, -settings.zoom, settings.zoom, settings.zNear, settings.zFar));
			cam->getFrameBuffer()->resize(settings.textureWidth, settings.textureHeight);
		}
		else if (mode == CameraMode::Perspective) {
			cam.reset(new PerspectiveCamera(settings.fov, settings.aspectRatio, settings.zNear, settings.zFar));
			cam->getFrameBuffer()->resize(settings.textureWidth, settings.textureHeight);
		}
	}
}