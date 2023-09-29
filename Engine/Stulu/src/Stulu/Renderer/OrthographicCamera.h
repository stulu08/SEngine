#pragma once
#include <glm/glm.hpp>
#include "Stulu/Renderer/Camera.h"

namespace Stulu {
	class STULU_API OrthographicCamera : public Camera {
	public:
		OrthographicCamera(float left, float right, float bottom, float top, float zNear, float zFar, const FrameBufferSpecs& specs = FrameBufferSpecs());
		virtual ~OrthographicCamera();

		const virtual void setProjection(float left, float right, float bottom, float top, float zNear, float zFar) override;
		const glm::mat4& getProjectionMatrix() const override { return m_projcetionMatrix; }

		const virtual Ref<FrameBuffer>& getFrameBuffer() override { return m_frameBuffer; }
		const virtual void bindFrameBuffer() const override;
		const virtual void unbindFrameBuffer() const override;
	private:
		glm::mat4 m_projcetionMatrix;
		Ref<FrameBuffer> m_frameBuffer;
	};
}