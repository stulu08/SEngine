#pragma once
#include "Stulu/Renderer/ShaderCompiler.h"

namespace Stulu {
	class STULU_API SpirVShaderCompiler : public ShaderCompiler {
	public:
		SpirVShaderCompiler();

		virtual void Compile(const ShaderSource& sources, ShaderCompileResult& result) const override;
		virtual void CompileToCache(const ShaderSource& sources, const std::string& cacheFile, ShaderCompileResult& result) const override;

		virtual void LoadFromCache(const std::string& cacheFile, ShaderCompileResult& result) const override;

		virtual bool isCacheUpToDate(const std::string& cacheFile, const std::string& shaderSourceFile) const override;

		virtual inline void AddHeader(const std::string& headers) override {
			m_headers.push_back(headers);
		}
		virtual inline void AddHeaderFront(const std::string& headers) override {
			m_headers.insert(m_headers.begin(), headers);
		}

		virtual inline const std::vector<std::string>& GetHeaders() const { return m_headers; }
		virtual inline std::vector<std::string>& GetHeaders() override { return m_headers; }
	private:
		static bool s_glslangInitlized;
		std::vector<std::string> m_headers;

		std::string ApplyHeaders(const std::string& src) const;
	};
}