#pragma once
#include <string>
#include <glm/glm.hpp>

namespace Stulu {
	class Shader {
	public:
		virtual ~Shader() {};

		virtual void bind() const = 0;
		virtual void unbind() const = 0;
		virtual const std::string& getName() const = 0;
		static Ref<Shader> create(const std::string& path);
		static Ref<Shader> create(const std::string& name, const std::string& vertex, const std::string& fragment);
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