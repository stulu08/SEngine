#pragma once
#include <string>
#include <glm/glm.hpp>

namespace Stulu {
	class Shader {
	public:
		virtual ~Shader() {};

		virtual void bind() const = 0;
		virtual void unbind() const = 0;
		virtual void uploadMat4Uniform(const std::string& name, const glm::mat4& matrix) = 0;

		static Shader* create(const std::string& vertex, const std::string& fragment);
	};
}