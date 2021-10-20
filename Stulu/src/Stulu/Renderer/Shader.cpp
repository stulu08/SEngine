#include "st_pch.h"
#include "Shader.h"
#include "Renderer.h"
#include "Platform/OpenGL/OpenGLShader.h"
#include "Platform/Vulkan/VulkanShader.h"

namespace Stulu{
	Ref<Shader> Shader::create(const std::string& path) {
		switch (Renderer::getRendererAPI())
		{
		case RenderAPI::API::OpenGL:
			return std::make_shared<OpenGLShader>(path);
		case RenderAPI::API::Vulkan:
			return std::make_shared<VulkanShader>(path);
		case RenderAPI::API::none:
			CORE_ASSERT(false, "No renderAPI specified")
				return nullptr;
		default:
			CORE_ASSERT(false, "RenderAPI not suported")
				return nullptr;
		}

		CORE_ASSERT(false, "Unknown error in Shader creation")
			return nullptr;
	}
	Ref<Shader> Shader::create(const std::string& name, const std::string& vertex, const std::string& fragment) {
		switch (Renderer::getRendererAPI())
		{
		case RenderAPI::API::OpenGL:
			return std::make_shared<OpenGLShader>(name, vertex, fragment);
		case RenderAPI::API::Vulkan:
			return std::make_shared<VulkanShader>(name, vertex, fragment);
		case RenderAPI::API::none:
			CORE_ASSERT(false, "No renderAPI specified")
				return nullptr;
		default:
			CORE_ASSERT(false, "RenderAPI not suported")
				return nullptr;
		}

		CORE_ASSERT(false, "Unknown error in Shader creation")
			return nullptr;
	}
	void ShaderLibary::add(const std::string& name, const Ref<Shader>& shader) {
		CORE_ASSERT(!exists(name), "{0} Shader already exists: {1}", name);
		m_shaders[name] = shader;
	}
	bool ShaderLibary::exists(const std::string& name)
	{
		return m_shaders.find(name) != m_shaders.end();
	}
	void ShaderLibary::add(const Ref<Shader>& shader){
		const std::string& name = shader->getName();
		add(name, shader);
	}
	Ref<Shader> ShaderLibary::load(const std::string& path){
		auto shader = Shader::create(path);
		add(shader);
		return shader;
	}
	Ref<Shader> ShaderLibary::load(const std::string& name, const std::string& path){

		auto shader = Shader::create(path);
		add(name, shader);
		return shader;
	}
	Ref<Shader> ShaderLibary::get(const std::string& name){
		CORE_ASSERT(exists(name), std::string("Shader not found: ") + name);
		return m_shaders[name];
	}
}