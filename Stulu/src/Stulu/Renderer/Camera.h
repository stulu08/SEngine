#pragma once
#include <glm/glm.hpp>
namespace Stulu {
	enum CameraMode { Perspective = 0, Orthographic = 1 };
	class Camera {
	public:
		virtual ~Camera(){}
		const virtual void setProjection(float fov, float aspect, float z_near, float z_far) {};
		const virtual void setProjection(float left, float right, float bottom, float top, float zNear, float zFar) {};
		const virtual glm::mat4& getProjectionMatrix() const = 0;
	};

}