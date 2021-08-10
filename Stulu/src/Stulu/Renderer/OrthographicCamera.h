#pragma once
#include <glm/glm.hpp>
#include "Stulu/Renderer/Camera.h"

namespace Stulu {
	class OrthographicCamera : public Camera
	{
	public:
		OrthographicCamera(float left, float right, float bottom, float top);

		void setProjection(float left, float right, float bottom, float top);

		const glm::mat4& getProjectionMatrix() const override { return m_projcetionMatrix; }
		const glm::mat4& getViewMatrix() const override { return m_viewMatrix; }
		const glm::mat4& getViewProjectionMatrix() const override { return m_viewProjcetionMatrix; }

		glm::vec3& getPosition() { return m_position; }
		void setPosition(const glm::vec3& position) { m_position = position; recalculateViewMatrix(); }

		float getRotation() const { return m_rotation; }
		void setRotation(float rotation) { m_rotation = rotation; recalculateViewMatrix(); }
	private:
		void recalculateViewMatrix();


		glm::mat4 m_projcetionMatrix;
		glm::mat4 m_viewMatrix;
		glm::mat4 m_viewProjcetionMatrix;

		glm::vec3 m_position = { 0.0f, 0.0f, 0.0f };
		float m_rotation = 0.0f;
	};
}