#pragma once
#include <string>
#include <glm/glm.hpp>

namespace Stulu {
	class Shader {
	public:
		virtual ~Shader() {};

		virtual void bind() const = 0;
		virtual void unbind() const = 0;

		static Shader* create(const std::string& path, bool multiFile = false);
		static Shader* create(const std::string& vertex, const std::string& fragment);
	};
}