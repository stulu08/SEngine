#pragma once
#include "Shader.h"

#include "Material/MaterialProperty.h"

#include <map>

namespace Stulu {
	class STULU_API ShaderEntry {
	public:
		ShaderEntry(const Ref<Shader>& shader, const std::string& path);
		ShaderEntry(const Ref<Shader>& shader, const std::vector<Ref<MaterialProperty>>& props, const std::string& path);

		bool HasProperity(std::string name) const;
		std::vector<Ref<MaterialProperty>>& GetProperities() { return m_props; }
		const std::vector<Ref<MaterialProperty>>& GetProperities() const { return m_props; }
		Ref<MaterialProperty> GetProperity(std::string name) const;

		inline Ref<Shader> GetShader() const { return m_shader; }

		inline std::string GetPath() const { return m_path; }
	private:
		Ref<Shader> m_shader;
		std::vector<Ref<MaterialProperty>> m_props;
		std::string m_path;

		friend class ShaderSystem;
	};

	class STULU_API ShaderSystem {
	public:
		ShaderSystem();
		~ShaderSystem();

		Ref<Shader> AddShader(const std::string& path);
		void LoadAllShaders(const std::string& path);
		
		// loads a file from the internal shader system or relative to the include directories
		std::string LoadShaderSource(const std::string& shaderStr) const;

		Ref<Shader> CreateShader(const std::string& name, const ShaderSource& source, const std::vector<Ref<MaterialProperty>>& properties = {}, const std::string& path = "");

		inline Ref<Shader> CreateShader(const std::string& name, const std::string& vertex, const std::string& fragment) {
			return CreateShader(name, ShaderSource{ vertex, fragment });
		}
		inline Ref<Shader> CreateShader(const std::string& name, const std::string& compute) {
			return CreateShader(name, ShaderSource{ compute });
		}
		inline void AddInternalIncludeFile(const std::string& name, const std::string& content) { m_internalFiles[name] = content; }
		inline void AddIncludePath(const std::string& path) { m_includeDirs.push_back(path); }

		inline Ref<ShaderEntry> GetEntry(const std::string& name) const {
			if(m_shaders.find(name) != m_shaders.end())
				return m_shaders.at(name);

			CORE_ERROR("Shader {0} not found!", name);
			return nullptr;
		}
		inline Ref<Shader> GetShader(const std::string& name) const { 
			if (m_shaders.find(name) != m_shaders.end())
				return m_shaders.at(name)->GetShader();

			CORE_ERROR("Shader {0} not found!", name);
			return nullptr;
		}

		inline void RemoveShader(const std::string& name) { m_shaders.erase(name); }

		inline std::map<std::string, Ref<ShaderEntry>> GetShaders() { return m_shaders; }

		void ReloadShaders();
		void ReloadShader(const std::string& name);

		static bool CheckSpirv();
		inline bool SpirvSupported() const { return m_spirvSupported; }

		inline void SetCacheFolder(const std::string& cache) {
			m_cacheFolder = cache;
		}

		inline std::string BuildCacheFile(const std::string& name) const {
			return m_cacheFolder + "/" + name + ".cached-shader";
		}
	private:
		Ref<ShaderCompiler> m_compiler;
		std::string m_cacheFolder;
		std::map<std::string, Ref<ShaderEntry>> m_shaders;
		std::vector<std::string> m_includeDirs;
		std::unordered_map<std::string, std::string> m_internalFiles;
		bool m_spirvSupported;

		struct ShaderIncludeNode {
			std::string filename;
			std::vector<std::string> lines;
			std::vector<std::pair<size_t, ShaderIncludeNode>> includes;
		};

		std::string ProcessShader(const std::string& fileName) const;
		ShaderIncludeNode ParseShaderIncludeTree(const std::string& filepath) const;
		void FlattenShaderInclude(const ShaderIncludeNode& node, std::ostream& out, const std::string& indent = "") const;

		std::string GetShaderName(const std::string& source) const;
		std::vector<Ref<MaterialProperty>> ProcessProperties(std::string& source) const;
		ShaderSource ProcessRegions(std::string& source) const;
	};

#define ST_ShaderViewFlags_DisplayLighting ((uint32_t)ShaderViewFlags::DisplayLighting)
#define ST_ShaderViewFlags_DisplayDiffuse ((uint32_t)ShaderViewFlags::DisplayDiffuse)
#define ST_ShaderViewFlags_DisplaySpecular ((uint32_t)ShaderViewFlags::DisplaySpecular)
#define ST_ShaderViewFlags_DisplayNormal ((uint32_t)ShaderViewFlags::DisplayNormal)
#define ST_ShaderViewFlags_DisplayRoughness ((uint32_t)ShaderViewFlags::DisplayRoughness)
#define ST_ShaderViewFlags_DisplayMetallic ((uint32_t)ShaderViewFlags::DisplayMetallic)
#define ST_ShaderViewFlags_DisplayAmbient ((uint32_t)ShaderViewFlags::DisplayAmbient)
#define ST_ShaderViewFlags_DisplayTexCoords ((uint32_t)ShaderViewFlags::DisplayTexCoords)
#define ST_ShaderViewFlags_DisplayVertices ((uint32_t)ShaderViewFlags::DisplayVertices)
#define ST_ShaderViewFlags_DisplayEmission ((uint32_t)ShaderViewFlags::DisplayEmission)
#define ST_ShaderViewFlags_DisplayOcclusion ((uint32_t)ShaderViewFlags::DisplayAmbientOcclusion)
#define ST_ShaderViewFlags_DisplayDepth ((uint32_t)ShaderViewFlags::DisplayDepth)
	static inline constexpr int32_t ST_MAXLIGHTS = 50;
	// how my shadow casters 
	static inline constexpr int32_t ST_MAX_SHADOW_CASTERS = 50;

	static inline constexpr int32_t ST_MAX_SHADOW_CASCADES = 6;

	enum class ShaderViewFlags {
		DisplayLighting = 1 << 0,
		//the following will only display itself(DisplayNormal will only display normal map)
		DisplayDiffuse = 1 << 1,
		DisplaySpecular = 1 << 2,
		DisplayNormal = 1 << 3,
		DisplayRoughness = 1 << 4,
		DisplayMetallic = 1 << 5,
		DisplayAmbient = 1 << 6,
		DisplayTexCoords = 1 << 7,
		DisplayVertices = 1 << 8,
		DisplayEmission = 1 << 9,
		DisplayAmbientOcclusion = 1 << 10,
		DisplayDepth = 1 << 11,
	};
}



