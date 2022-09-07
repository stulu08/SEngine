#pragma once
#include "Stulu/Renderer/FrameBuffer.h"
namespace Stulu {
	enum class CameraMode { Perspective = 0, Orthographic = 1 };
	class STULU_API Camera {
	public:
		virtual ~Camera(){}
		const virtual void setProjection(float fov, float aspect, float z_near, float z_far) {};
		const virtual void setProjection(float left, float right, float bottom, float top, float zNear, float zFar) {};
		const virtual glm::mat4& getProjectionMatrix() const = 0;
		
		const virtual Ref<FrameBuffer>& getFrameBuffer() = 0;
		const virtual void bindFrameBuffer() const = 0;
		const virtual void unbindFrameBuffer() const = 0;
	};
}