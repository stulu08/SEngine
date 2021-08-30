#pragma once
namespace Stulu {
	class Camera {
	public:
		virtual ~Camera(){}
		const virtual void setProjection(float fov, float aspect, float z_near, float z_far) {};
		const virtual void setProjection(float left, float right, float bottom, float top, float zNear, float zFar) {};
		const virtual glm::mat4& getProjectionMatrix() const = 0;
		const virtual glm::mat4& getViewMatrix() const = 0;
		const virtual glm::mat4& getViewProjectionMatrix() const = 0;
		const virtual glm::vec3& getPosition() = 0;
		const virtual glm::vec3& getRotation() = 0;
		const virtual void setPosition(glm::vec3& pos) = 0;
		const virtual void setRotation(glm::vec3& rotation) = 0;

		const virtual glm::vec3 getUpDirection() = 0;
		const virtual glm::vec3 getRightDirection() = 0;
		const virtual glm::vec3 getForwardDirection() = 0;
	};
}