#pragma once
#include "Stulu/Renderer/Camera.h"
#include "Component.h"
namespace Stulu {
	enum class ClearType { Color = 0, Skybox = 1 };

	class STULU_API Component;
	class STULU_API CameraComponent : public Component {
	public:

		CameraComponent();
		CameraComponent(CameraMode mode, uint32_t width, uint32_t height);
		CameraComponent(const CameraComponent&);

		void ResizeTexture(uint32_t width, uint32_t height);
		void UpdateCamera();
		Frustum CalculateFrustum(TransformComponent& transform) const;

		inline bool IsRenderTarget() const {
			return m_renderTarget != UUID::null;
		}

		inline const glm::mat4& GetProjection() const { return m_cam.getProjectionMatrix(); }
		inline Camera& GetNativeCamera() { return m_cam; }
		inline const Camera& GetNativeCamera() const { return m_cam; }
		inline const Ref<FrameBuffer> GetFrameBuffer() const { return m_cam.getFrameBuffer(); }
		inline const Ref<Texture2D> GetTexture() const { return m_cam.getFrameBuffer()->getColorAttachment(); }

		inline uint32_t GetTargetWidth() const { return GetTexture()->getWidth(); }
		inline uint32_t GetTargetHeight() const { return GetTexture()->getHeight(); }
		inline CameraMode GetMode() const { return m_mode; }
		inline ClearType GetClearType() const { return m_clearType; }
		inline float GetNear() const { return m_near; }
		inline float GetFar() const { return m_far; }
		inline float GetFov() const { return m_fov; }
		inline float GetZoom() const { return m_fov; }
		inline float GetAspect() const { return (float)GetTargetWidth() / (float)GetTargetHeight(); }
		inline int32_t GetDepth() const { return m_depth; }
		inline glm::vec4 GetClearColor() const { return m_clearColor; }

		inline void SetMode(CameraMode value) {
			m_mode = value;
		}
		inline void SetClearType(ClearType value) {
			m_clearType = value;
		}
		inline void SetNear(float value) {
			m_near = value;
		}
		inline void SetFar(float value) {
			m_far = value;
		}
		inline void SetFov(float value) {
			m_fov = value;
		}
		inline void SetZoom(float value) {
			m_fov = value;
		}
		inline void SetDepth(int32_t value) {
			m_depth = value;
		}
		inline void SetClearColor(const glm::vec4& value) {
			m_clearColor = value;
		}

		// internal for scene renderer
		CameraComponent(const Camera& camera);

		virtual void Serialize(YAML::Emitter& out) const override {
			out << YAML::Key << "m_mode" << YAML::Value << (int)m_mode;
			out << YAML::Key << "m_clearType" << YAML::Value << (int)m_clearType;
			out << YAML::Key << "m_depth" << YAML::Value << m_depth;
			out << YAML::Key << "m_fov" << YAML::Value << m_fov;
			out << YAML::Key << "m_far" << YAML::Value << m_far;
			out << YAML::Key << "m_clearColor" << YAML::Value << m_clearColor;
			out << YAML::Key << "m_renderTarget" << YAML::Value << m_renderTarget;
		}
		virtual void Deserialize(YAML::Node& node) override {
			if (node["m_mode"])
				m_mode = (CameraMode)node["m_mode"].as<int>();
			if (node["m_clearType"])
				m_clearType = (ClearType)node["m_clearType"].as<int>();
			if (node["m_depth"])
				m_depth = node["m_depth"].as<int32_t>();
			if (node["m_fov"])
				m_fov = node["m_fov"].as<float>();
			if (node["m_near"])
				m_near = node["m_near"].as<float>();
			if (node["m_far"])
				m_far = node["m_far"].as<float>();
			if (node["m_clearColor"])
				m_clearColor = node["m_clearColor"].as<glm::vec4>();
			if (node["m_renderTarget"])
				m_renderTarget = node["m_renderTarget"].as<UUID>();

			UpdateCamera();
		}
	private:
		CameraMode m_mode;
		ClearType m_clearType = ClearType::Color;
		int32_t m_depth = 1;
		float m_fov = 80.0f, m_near = 0.01f, m_far = 1000.0f;
		glm::vec4 m_clearColor = { 0.0f, 0.0f, 0.0f, 1.0f };
		// render target only
		UUID m_renderTarget = UUID::null;
		Camera m_cam;

		friend class Scene;
		friend class SceneRenderer;
	};
}