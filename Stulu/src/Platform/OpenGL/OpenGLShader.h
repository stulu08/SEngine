#pragma once
#include "Stulu/Renderer/Shader.h"

namespace Stulu {
	class OpenGLShader : public Shader
	{
	public:
		OpenGLShader(const std::string& vertex, const std::string& fragment);
		virtual ~OpenGLShader();

		virtual void bind() const override;
		virtual void unbind() const override;
		virtual void uploadMat4Uniform(const std::string& name,const glm::mat4& matrix) override;

	private:
		uint32_t m_rendererID;
	};
}

