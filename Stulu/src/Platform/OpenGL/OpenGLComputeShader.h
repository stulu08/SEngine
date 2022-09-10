#pragma once
#include "Stulu/Renderer/Shader.h"

typedef unsigned int GLenum;
namespace Stulu {

	class STULU_API OpenGLComputeShader : public ComputeShader
	{
	public:
		OpenGLComputeShader(const std::string& name, const std::string& src);
		virtual ~OpenGLComputeShader();

		virtual void reload(const std::string& src) override;
		virtual void Dispatch(const glm::uvec3& numWorkGroups = {1u,1u,1u}, uint32_t usage = 0xFFFFFFFF) override;

		virtual void setTexture(const std::string& name, uint32_t binding, const Ref<Texture>& texture, uint32_t mipLevel = 0, AccesMode mode = AccesMode::ReadWrite) override;
		virtual void setFloat(const std::string& name, float value) override;
		virtual void setInt(const std::string& name, int value) override;
		virtual void setVec(const std::string& name, const glm::vec4& value) override;
		virtual void setMat(const std::string& name, const glm::mat4& value) override;
		virtual void setFloatArray(const std::string& name, const float* floats) override;
		virtual void setIntArray(const std::string& name, const int* ints) override;
		virtual void setVecArray(const std::string& name, const glm::vec4* vecs) override;
		virtual void setMatArray(const std::string& name, const glm::mat4* mats) override;

		virtual const glm::ivec3 getMaxWorkGroupCount() const override;
		virtual const glm::ivec3 getMaxWorkGroupSizes() const override;
		virtual const uint32_t getMaxWorkGroupInvocationCount() const override;
		virtual const std::string& getName() const override { return m_name; }
		virtual const std::string& getSource() const override { return m_source; }
		virtual const void* getNativeRendererObject() const override { return &m_rendererID; };
	private:
		uint32_t m_rendererID = 0;
		std::string m_name = "";
		std::string m_source = "";

		void compile(const std::string& src);
	};
}

