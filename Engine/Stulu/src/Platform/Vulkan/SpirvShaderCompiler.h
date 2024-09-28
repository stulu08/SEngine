#pragma once
#include "Stulu/Renderer/ShaderCompiler.h"

namespace Stulu {
	class STULU_API SpirVShaderCompiler : public ShaderCompiler {
	public:
		SpirVShaderCompiler();

		virtual ShaderCompileResult Compile(const ShaderSource& sources) const override;
		virtual ShaderCompileResult CompileToCache(const ShaderSource& sources, const std::string& cacheFile) const override;

		virtual ShaderCompileResult LoadFromCache(const std::string& cacheFile) const override;

		virtual bool isCacheUpToDate(const std::string& cacheFile, const std::string& shaderSourceFile) const override;
	private:
		static bool s_glslangInitlized;
	};
}