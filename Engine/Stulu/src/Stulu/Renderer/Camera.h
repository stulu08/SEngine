#pragma once
#include "Stulu/Renderer/FrameBuffer.h"
namespace Stulu {

	enum class CameraMode : uint8_t {
		Perspective = 0,
		Orthographic = 1
	};

	class Camera {
	public:
		Camera(CameraMode mode, const FrameBufferSpecs specs = FrameBufferSpecs(), bool hasResultBuffer = true) 
			: m_mode(mode), m_frameBuffer(nullptr), m_resultFrameBuffer(), m_projcetionMatrix(1.0f) {
			m_frameBuffer = FrameBuffer::create(specs);
			if (hasResultBuffer) {
				FrameBufferSpecs resultSpecs = specs;
				resultSpecs.samples = MSAASamples::Disabled;
				m_resultFrameBuffer = FrameBuffer::create(resultSpecs);
			}
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
			if (HasResultFrameBuffer())
				m_resultFrameBuffer->resize(width, height);
		}
		inline void ResizeFrameBuffer(uint32_t width, uint32_t height, MSAASamples samples) {
			m_frameBuffer->resize(width, height, samples);
			if (HasResultFrameBuffer())
				m_resultFrameBuffer->resize(width, height);
		}
		
		inline Ref<FrameBuffer> getRenderFrameBuffer() const {
			return m_frameBuffer;
		}
		inline bool HasResultFrameBuffer() const {
			return m_resultFrameBuffer != nullptr;
		}
		inline Ref<FrameBuffer> getResultFrameBuffer() const {
			return HasResultFrameBuffer() ? m_resultFrameBuffer : m_frameBuffer;
		}
	private:
		glm::mat4 m_projcetionMatrix;
		Ref<FrameBuffer> m_frameBuffer;
		Ref<FrameBuffer> m_resultFrameBuffer;
		CameraMode m_mode;
	};
}