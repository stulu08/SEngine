#pragma once
#include <string>
#include <glm/glm.hpp>

namespace Stulu {
	class Shader {
	public:
		virtual ~Shader() = default;

		virtual void bind() const = 0;
		virtual void unbind() const = 0;
		virtual const std::string& getName() const = 0;
		static Ref<Shader> create(const std::string& path);
		static Ref<Shader> create(const std::string& name, const std::string& vertex, const std::string& fragment);

		virtual void setMat4(const std::string& name, const glm::mat4& mat) = 0;
		virtual void setFloat4(const std::string& name, const glm::vec4& vec) = 0;
		virtual void setFloat3(const std::string& name, const glm::vec3& vec) = 0;
		virtual void setFloat2(const std::string& name, const glm::vec2& vec) = 0;
		virtual void setFloat(const std::string& name, const float Float) = 0;
		virtual void setInt(const std::string& name, const int32_t Int) = 0;
		virtual void setIntArray(const std::string& name, const int* values, uint32_t count) = 0;
	};
	class ShaderLibary {
	public:
		void add(const Ref<Shader>& shader);
		void add(const std::string& name, const Ref<Shader>& shader);
		bool exists(const std::string& name);
		Ref<Shader> load(const std::string& path);
		Ref<Shader> load(const std::string& name, const std::string& path);
		Ref<Shader> get(const std::string& name);
	private:
		std::unordered_map<std::string, Ref<Shader>> m_shaders;
	};
}