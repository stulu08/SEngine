#pragma once
#include <glm/glm.hpp>
#include "Stulu/Renderer/Camera.h"

namespace Stulu {
	class OrthographicCamera : public Camera
	{
	public:
		OrthographicCamera(float left, float right, float bottom, float top, float zNear, float zFar);

		const virtual void setProjection(float left, float right, float bottom, float top, float zNear, float zFar) override;

		const glm::mat4& getProjectionMatrix() const override { return m_projcetionMatrix; }
		const glm::mat4& getViewMatrix() const override { return m_viewMatrix; }
		const glm::mat4& getViewProjectionMatrix() const override { return m_viewProjcetionMatrix; }

		const virtual glm::vec3& getPosition() override { return m_position; } 
		const virtual glm::vec3& getRotation() override { return m_rotation; }
		const virtual void setPosition(glm::vec3& pos) override { m_position = pos; recalculateViewMatrix(); }
		const virtual void setRotation(glm::vec3& rotation) override { m_rotation = rotation; recalculateViewMatrix(); }

		const virtual glm::vec3 getUpDirection() override;
		const virtual glm::vec3 getRightDirection() override;
		const virtual glm::vec3 getForwardDirection() override;
	private:
		void recalculateViewMatrix();

		glm::mat4 m_projcetionMatrix;
		glm::mat4 m_viewMatrix;
		glm::mat4 m_viewProjcetionMatrix;

		glm::vec3& m_position = glm::vec3(.0f);
		glm::vec3& m_rotation = glm::vec3(.0f);

	};
}