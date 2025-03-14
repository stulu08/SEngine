#pragma once
#include "Stulu/Renderer/FrameBuffer.h"
namespace Stulu {

	enum class CameraMode : uint8_t {
		Perspective = 0,
		Orthographic = 1
	};

	class Camera {
	public:
		Camera(CameraMode mode, const FrameBufferSpecs specs = FrameBufferSpecs()) 
			: m_mode(mode), m_frameBuffer(nullptr), m_projcetionMatrix(1.0f){
			m_frameBuffer = FrameBuffer::create(specs);
		}
		~Camera() = default;
		
		inline void SetPrespective(float fov, float aspect, float z_near, float z_far) {
			m_projcetionMatrix = glm::perspective(glm::radians(fov), aspect, z_near, z_far);
			m_mode = CameraMode::Perspective;
		}
		inline void SetOrthographic(float left, float right, float bottom, float top, float zNear, float zFar) {
			m_projcetionMatrix = glm::ortho(left, right, bottom, top, zNear, zFar);
			m_mode = CameraMode::Orthographic;
		}
		inline CameraMode getMode() const {
			return m_mode;
		}
		inline const glm::mat4& getProjectionMatrix() const {
			return m_projcetionMatrix;
		}
		inline void ResizeFrameBuffer(uint32_t width, uint32_t height) {
			m_frameBuffer->resize(width, height);
		}

		inline Ref<FrameBuffer> getFrameBuffer() const {
			return m_frameBuffer;
		}
		inline void bindFrameBuffer() const {
			m_frameBuffer->bind();
		}
		inline void unbindFrameBuffer() const {
			m_frameBuffer->unbind();
		}
	private:
		glm::mat4 m_projcetionMatrix;
		Ref<FrameBuffer> m_frameBuffer;
		CameraMode m_mode;
	};
}