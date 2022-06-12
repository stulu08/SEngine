#include "st_pch.h"
#include "Camera.h"
#include "Stulu/Scene/Components/Components.h"
#include "Component.h"

namespace Stulu {
	CameraComponent::CameraComponent(const CameraMode mode)
		: mode(mode) {
		if (mode == CameraMode::Perspective) {
			cam = createRef<PerspectiveCamera>(settings.fov, settings.aspectRatio, settings.zNear, settings.zFar);
		}
		else if (mode == CameraMode::Orthographic) {
			cam = createRef<OrthographicCamera>(-settings.aspectRatio * settings.zoom, settings.aspectRatio * settings.zoom, -settings.zoom, settings.zoom, settings.zNear, settings.zFar);
		}
			

		if (reflectionMap == nullptr)
			reflectionMap = CubeMap::create(ST_REFLECTION_MAP_SIZE);
		if (reflectionFrameBuffer == nullptr)
			reflectionFrameBuffer = FrameBuffer::create(FrameBufferSpecs{ ST_REFLECTION_MAP_SIZE,ST_REFLECTION_MAP_SIZE,1,false });
	}
	const void CameraComponent::onResize(uint32_t width, uint32_t height) {
		ST_PROFILING_FUNCTION();
		settings.aspectRatio = (float)width / (float)height;
		settings.textureWidth = width;
		settings.textureHeight = height;
		cam->getFrameBuffer()->getSpecs().textureFormat = TextureSettings::Format::RGBA16F;
		cam->getFrameBuffer()->resize(width, height);
		updateProjection();
		if (reflectionMap == nullptr)
			reflectionMap = CubeMap::create(ST_REFLECTION_MAP_SIZE);
		if (reflectionFrameBuffer == nullptr)
			reflectionFrameBuffer = FrameBuffer::create(FrameBufferSpecs{ ST_REFLECTION_MAP_SIZE,ST_REFLECTION_MAP_SIZE,1,false });
	}
	const void CameraComponent::updateSize() {
		ST_PROFILING_FUNCTION();
		settings.aspectRatio = (float)settings.textureWidth / (float)settings.textureHeight;
		cam->getFrameBuffer()->resize(settings.textureWidth, settings.textureHeight);
		updateProjection();
		if (reflectionMap == nullptr)
			reflectionMap = CubeMap::create(ST_REFLECTION_MAP_SIZE);
		if (reflectionFrameBuffer == nullptr)
			reflectionFrameBuffer = FrameBuffer::create(FrameBufferSpecs{ ST_REFLECTION_MAP_SIZE,ST_REFLECTION_MAP_SIZE,1,false });
	}
	const void CameraComponent::updateProjection() {
		ST_PROFILING_FUNCTION();
		if (mode == CameraMode::Perspective) {
			cam->setProjection(settings.fov, settings.aspectRatio, settings.zNear, settings.zFar);
		}
		else if (mode == CameraMode::Orthographic) {
			cam->setProjection(-settings.aspectRatio * settings.zoom, settings.aspectRatio * settings.zoom, -settings.zoom, settings.zoom, settings.zNear, settings.zFar);
		}
		if (reflectionMap == nullptr)
			reflectionMap = CubeMap::create(ST_REFLECTION_MAP_SIZE);
		if (reflectionFrameBuffer == nullptr)
			reflectionFrameBuffer = FrameBuffer::create(FrameBufferSpecs{ ST_REFLECTION_MAP_SIZE,ST_REFLECTION_MAP_SIZE,1,false });
	}
	const void CameraComponent::updateMode() {
		ST_PROFILING_FUNCTION();
		if (mode == CameraMode::Orthographic) {
			cam.reset(new OrthographicCamera(-settings.aspectRatio * settings.zoom, settings.aspectRatio * settings.zoom, -settings.zoom, settings.zoom, settings.zNear, settings.zFar));
			cam->getFrameBuffer()->resize(settings.textureWidth, settings.textureHeight);
		}
		else if (mode == CameraMode::Perspective) {
			cam.reset(new PerspectiveCamera(settings.fov, settings.aspectRatio, settings.zNear, settings.zFar));
			cam->getFrameBuffer()->resize(settings.textureWidth, settings.textureHeight);
		}
		if (reflectionMap == nullptr)
			reflectionMap = CubeMap::create(ST_REFLECTION_MAP_SIZE);
		if (reflectionFrameBuffer == nullptr)
			reflectionFrameBuffer = FrameBuffer::create(FrameBufferSpecs{ ST_REFLECTION_MAP_SIZE,ST_REFLECTION_MAP_SIZE,1,false });
	}
}