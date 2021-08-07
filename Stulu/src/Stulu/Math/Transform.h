#pragma once
#include <glm/glm.hpp>

namespace Stulu {
	class Transform {
	public:
		Transform()
			: m_pos(0.0f), m_rotation(0.0f), m_scale(1.0f){}
		Transform(glm::vec2 pos, glm::vec3 rotation = { 0.0f, 0.0f, 0.0f }, glm::vec2 size = { 1.0f, 1.0f})
			: m_pos(glm::vec3(pos.x,pos.y,0.0f)), m_rotation(glm::vec3(rotation.x, rotation.y, 0.0f)), m_scale(glm::vec3(size.x, size.y, 1.0f)) {};
		Transform(glm::vec3 pos, glm::vec3 rotation = { 0.0f, 0.0f, 0.0f }, glm::vec2 size = { 1.0f, 1.0f})
			: m_pos(pos), m_rotation(glm::vec3(rotation.x, rotation.y, 0.0f)), m_scale(glm::vec3(size.x, size.y, 1.0f)) {};
		Transform(glm::vec3 pos, glm::vec3 rotation = { 0.0f, 0.0f, 0.0f }, glm::vec3 scale = { 1.0f, 1.0f, 1.0f })
			: m_pos(pos), m_rotation(rotation), m_scale(scale) {};

		inline void setPos(glm::vec3 pos) { m_pos = pos; }
		inline void setRotation(glm::vec3 rotation) { m_rotation = rotation; }
		inline void setScale(glm::vec3 scale) { m_scale = scale; }

		inline glm::vec3& getPos() { return m_pos; }
		inline glm::vec3& getRotation() { return m_rotation; }
		inline glm::vec3& getScale() { return m_scale; }
		const glm::mat4& getMat4();

		void reset();

		static void ImGuiTransformEdit(Transform& transform, const char* name);
	private:
		glm::vec3 m_pos;
		glm::vec3 m_rotation;
		glm::vec3 m_scale;
	};
}