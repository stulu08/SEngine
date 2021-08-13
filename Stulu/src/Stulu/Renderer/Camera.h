#pragma once
namespace Stulu {
	class Camera {
	public:
		virtual ~Camera(){}
		const virtual glm::mat4& getProjectionMatrix() const = 0;
		const virtual glm::mat4& getViewMatrix() const = 0;
		const virtual glm::mat4& getViewProjectionMatrix() const = 0;
		const virtual glm::vec3& getPos() = 0;
	};
}