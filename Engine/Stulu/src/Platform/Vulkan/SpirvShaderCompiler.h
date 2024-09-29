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
	private:
		static bool s_glslangInitlized;
	};
}