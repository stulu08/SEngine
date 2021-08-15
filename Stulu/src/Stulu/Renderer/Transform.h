#pragma once
#include <glm/glm.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
namespace Stulu {
	class Transform {
	public:
		glm::vec3 position;
		glm::vec3 rotation;
		glm::vec3 scale;

		Transform()
			: position(0.0f), rotation(0.0f), scale(1.0f){}

		Transform(glm::vec2 pos, glm::vec3 rotation = { 0.0f, 0.0f, 0.0f }, glm::vec2 size = { 1.0f, 1.0f})
			: position(glm::vec3(pos.x,pos.y,0.0f)), 
			  rotation(rotation), 
			  scale(glm::vec3(size.x, size.y, 1.0f)) {};

		Transform(glm::vec3 pos, glm::vec3 rotation = { 0.0f, 0.0f, 0.0f }, glm::vec3 scale = { 1.0f, 1.0f, 1.0f })
			: position(pos), rotation(rotation), scale(scale) {};
		inline void setRotation(float x, float y, float z) { rotation = glm::degrees(glm::vec3(x,y,z)); }
		const glm::quat getOrientation();
		const glm::mat4 toMat4();
		void reset();


		operator const glm::mat4() const {
			return glm::translate(glm::mat4(1.0f), position)
				* glm::toMat4(glm::quat(glm::radians(rotation)))
				* glm::scale(glm::mat4(1.0f), scale); }

		[[deprecated]]
		inline void setPos(glm::vec3 pos) { position = pos; }
		[[deprecated]]
		inline void setRotation(glm::vec3 rotation) { rotation = rotation; }
		[[deprecated]]
		inline void setScale(glm::vec3 scale) { scale = scale; }
		[[deprecated]]
		inline glm::vec3& getPos() { return position; }
		[[deprecated]]
		inline glm::vec3& getRotation() { return rotation; }
		[[deprecated]]
		inline glm::vec3& getScale() { return scale; }
	};
}