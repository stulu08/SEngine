#include "st_pch.h"
#include "OrthographicCamera.h"
#include "Stulu/Math/Math.h"
#include "Stulu/Renderer/FrameBuffer.h"
#include <glm/gtc/matrix_transform.hpp>

namespace Stulu{
	OrthographicCamera::OrthographicCamera(float left, float right, float bottom, float top, float zNear, float zFar, const FrameBufferSpecs& specs)
		:m_projcetionMatrix(glm::ortho(left, right, bottom, top, zNear, zFar)) {
		ST_PROFILING_FUNCTION();
		m_frameBuffer = FrameBuffer::create(specs);
	}
	OrthographicCamera::~OrthographicCamera() { }
	const void OrthographicCamera::setProjection(float left, float right, float bottom, float top, float zNear, float zFar){
		m_projcetionMatrix = glm::ortho(left, right, bottom, top, zNear, zFar);
	}
	const void OrthographicCamera::bindFrameBuffer() const {
		m_frameBuffer->bind();
	}
	const void OrthographicCamera::unbindFrameBuffer() const {
		m_frameBuffer->unbind();
	}
}