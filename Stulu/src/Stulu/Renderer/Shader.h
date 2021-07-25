#pragma once
#include <string>

namespace Stulu {
	class Shader {
	public:
		Shader(const std::string& vertex, const std::string& fragment);
		~Shader();


		void bind() const;
		void unbind() const;

	private:
		uint32_t m_rendererID;
	};
}