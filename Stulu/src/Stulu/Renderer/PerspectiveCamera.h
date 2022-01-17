#pragma once
#include <glm/glm.hpp>
#include "Stulu/Renderer/Camera.h"

namespace Stulu {
	class STULU_API PerspectiveCamera : public Camera{
	public:
		PerspectiveCamera(float fov, float aspect, float z_near, float z_far);
		const virtual void setProjection(float fov, float aspect, float z_near, float z_far) override;
		const glm::mat4& getProjectionMatrix() const override { return m_projcetionMatrix; }

		const virtual Ref<FrameBuffer>& getFrameBuffer() override { return m_frameBuffer; }
		const virtual void bindFrameBuffer() const override;
		const virtual void unbindFrameBuffer() const override;
	private:
		glm::mat4 m_projcetionMatrix;
		Ref<FrameBuffer> m_frameBuffer;
	};
}


