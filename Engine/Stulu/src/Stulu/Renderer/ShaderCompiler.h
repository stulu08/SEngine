#pragma once
#include "Stulu/Core/Log.h"

#include <vector>
#include <string>
#include <algorithm>

namespace Stulu {
	enum class ShaderType : uint32_t {
		None, Vertex, Fragment, Compute
	};

	inline ShaderType ShaderTypeFromString(std::string type) {
		std::transform(type.begin(), type.end(), type.begin(), [](char c) -> char { return std::toupper(c); });

		if (type == "VERTEX" || type == "VERT")
			return ShaderType::Vertex;
		else if (type == "FRAGMENT" || type == "FRAG")
			return ShaderType::Fragment;
		else if (type == "COMPUTE" || type == "COMP")
			return ShaderType::Compute;

		CORE_ERROR("Unknown Shadertype: {0}", type);
		return ShaderType::None;
	}
	inline std::string ShaderTypeToString(ShaderType type) {
		switch (type)
		{
		case Stulu::ShaderType::Vertex:
			return "vert";
		case Stulu::ShaderType::Fragment:
			return "frag";
		case Stulu::ShaderType::Compute:
			return "comp";
		}
		return "null";
	}

	struct CompiledShaderData {
		CompiledShaderData(const CompiledShaderData&) = default;
		CompiledShaderData(CompiledShaderData&&) = default;

		std::vector<uint32_t> data;
	};

	struct ShaderCompileResult {
	public:
		ShaderCompileResult() = default;
		ShaderCompileResult(ShaderCompileResult&) = default;
		ShaderCompileResult(ShaderCompileResult&&) = default;

		inline void Add(ShaderType type, CompiledShaderData&& source) {
			m_data.push_back({ type, std::move(source) });
		}
		inline void Add(ShaderType type, const CompiledShaderData& source) {
			m_data.push_back({ type, source });
		}

		inline size_t Size() const { return m_data.size(); }

		inline const std::pair<ShaderType, CompiledShaderData>& Get(size_t index) const { return m_data.at(index); }
		inline std::pair<ShaderType, CompiledShaderData>& Get(size_t index) { return m_data[index]; }
		inline std::pair<ShaderType, CompiledShaderData>& operator[](size_t index) { return m_data[index]; }
	private:
		std::vector<std::pair<ShaderType, CompiledShaderData>> m_data;
	};

	struct ShaderSource {
	public:
		ShaderSource() {

		}
		ShaderSource(const std::string& vertex, const std::string& fragment) {
			Add(ShaderType::Vertex, vertex);
			Add(ShaderType::Fragment, fragment);
		}
		ShaderSource(const std::string& compute) {
			Add(ShaderType::Compute, compute);
		}

		inline void Add(ShaderType type, const std::string& source) {
			m_sources.push_back({ type, source });
		}

		inline size_t Size() const { return m_sources.size(); }

		inline const std::pair<ShaderType, std::string>& Get(size_t index) const { return m_sources.at(index); }
		inline std::pair<ShaderType, std::string>& Get(size_t index) { return m_sources[index]; }
		inline std::pair<ShaderType, std::string>& operator[](size_t index) { return m_sources[index]; }
	private:
		std::vector<std::pair<ShaderType, std::string>> m_sources;
	};

	class STULU_API ShaderCompiler {
	public:
		virtual ~ShaderCompiler() = default;


		virtual bool Compile(const ShaderSource& sources, ShaderCompileResult& result) const = 0;
		virtual bool CompileToCache(const ShaderSource& sources, const std::string& cacheFile, ShaderCompileResult& result) const = 0;

		virtual bool LoadFromCache(const std::string& cacheFile, ShaderCompileResult& result) const = 0;
		
		virtual bool isCacheUpToDate(const std::string& cacheFile, const std::string& shaderSourceFile) const = 0;
		
		virtual void AddHeader(const std::string& header) = 0;
		virtual void AddHeaderFront(const std::string& header) = 0;
		virtual const std::vector<std::string>& GetHeaders() const = 0;
		virtual std::vector<std::string>& GetHeaders() = 0;

		static Ref<ShaderCompiler> Create();
	};
}

namespace std {
	inline string to_string(Stulu::ShaderType shaderType) {
		return Stulu::ShaderTypeToString(shaderType);
	}
}