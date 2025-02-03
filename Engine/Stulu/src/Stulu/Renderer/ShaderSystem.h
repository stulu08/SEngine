#pragma once
#include "Shader.h"

#include <map>

namespace Stulu {
	class STULU_API ShaderProperity;

	class STULU_API ShaderEntry {
	public:
		ShaderEntry(const Ref<Shader>& shader, const std::string& path);
		ShaderEntry(const Ref<Shader>& shader, const std::vector<Ref<ShaderProperity>>& props, const std::string& path);

		bool HasProperity(std::string name) const;
		std::vector<Ref<ShaderProperity>>& GetProperities() { return m_props; }
		const std::vector<Ref<ShaderProperity>>& GetProperities() const { return m_props; }
		Ref<ShaderProperity> GetProperity(std::string name) const;

		inline Ref<Shader> GetShader() const { return m_shader; }

		inline std::string GetPath() const { return m_path; }
	private:
		Ref<Shader> m_shader;
		std::vector<Ref<ShaderProperity>> m_props;
		std::string m_path;

		friend class ShaderSystem;
	};

	class STULU_API ShaderSystem {
	public:
		ShaderSystem();
		~ShaderSystem();

		Ref<Shader> AddShader(const std::string& path);
		void LoadAllShaders(const std::string& path);
		
		void ProcessShader(std::string& source) const;

		Ref<Shader> CreateShader(const std::string& name, const ShaderSource& source, const std::vector<Ref<ShaderProperity>>& properties = {}, const std::string& path = "");

		inline Ref<Shader> CreateShader(const std::string& name, const std::string& vertex, const std::string& fragment) {
			return CreateShader(name, ShaderSource{ vertex, fragment });
		}
		inline Ref<Shader> CreateShader(const std::string& name, const std::string& compute) {
			return CreateShader(name, ShaderSource{ compute });
		}
		inline void AddInternalIncludeFile(const std::string& name, const std::string& content) { m_internalFiles[name] = content; }
		inline void AddIncludePath(const std::string& path) { m_includeDirs.push_back(path); }

		inline Ref<ShaderEntry> GetEntry(const std::string& name) const { return m_shaders.at(name); }
		inline Ref<Shader> GetShader(const std::string& name) const { return m_shaders.at(name)->GetShader(); }

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
		std::string GetShaderName(const std::string& source) const;
		std::vector<Ref<ShaderProperity>> ProcessProperties(std::string& source) const;
		ShaderSource ProcessRegions(std::string& source) const;

		Ref<ShaderCompiler> m_compiler;
		std::string m_cacheFolder;
		std::map<std::string, Ref<ShaderEntry>> m_shaders;
		std::vector<std::string> m_includeDirs;
		std::unordered_map<std::string, std::string> m_internalFiles;
		bool m_spirvSupported;
	};

	class STULU_API ShaderProperity {
	public:
		enum class Type {
			None, Color, Range, Enum, Marker
		};
		virtual Type getType() const = 0;
		virtual std::string getName() const = 0;

		static Type TypeFromString(const std::string& str);
	};

	class STULU_API ShaderProperityRange : public ShaderProperity {
	public:
		ShaderProperityRange(const std::string& name, const std::string& values);
		ShaderProperityRange(const std::string& name, const float min, const float max)
			: m_name(name), m_min(min), m_max(max) {}

		float getMin() const { return m_min; }
		float getMax() const { return m_max; }

		virtual Type getType() const override { return Type::Range; }
		virtual std::string getName() const override { return m_name; }
	private:
		std::string m_name;
		float m_min = .01f, m_max = 1.0f;
	};
	class STULU_API ShaderProperityColor : public ShaderProperity {
	public:
		ShaderProperityColor(const std::string& name, const std::string& values);
		ShaderProperityColor(const std::string& name, bool hdr)
			: m_name(name), m_hdr(hdr) {}

		bool isHDR() const { return m_hdr; }

		virtual Type getType() const override { return Type::Color; }
		virtual std::string getName() const override { return m_name; }
	private:
		std::string m_name;
		bool m_hdr = false;
	};
	class STULU_API ShaderProperityEnum : public ShaderProperity {
	public:
		ShaderProperityEnum(const std::string& name, const std::string& values);
		ShaderProperityEnum(const std::string& name, const std::vector<std::string>& names)
			: m_name(name), m_names(names) {}

		const std::vector<std::string>& getNames() const { return m_names; };

		virtual Type getType() const override { return Type::Enum; }
		virtual std::string getName() const override { return m_name; }
	private:
		std::string m_name;
		std::vector<std::string> m_names;
	};
	class STULU_API ShaderProperityMarker : public ShaderProperity {
	public:
		ShaderProperityMarker(const std::string& name, const std::string& text);

		const std::string& getText() const { return m_text; }

		virtual Type getType() const override { return Type::Marker; }
		virtual std::string getName() const override { return m_name; }
	private:
		std::string m_name;
		std::string m_text;
	};

#define ST_USER_TEXTURE_START 5
#define ST_USER_TEXTURE_END 15
#define ST_USER_TEXTURE_COUNT (ST_USER_TEXTURE_END - ST_USER_TEXTURE_START)

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
#define ST_ShaderViewFlags_DisplayDepth ((uint32_t)ShaderViewFlags::DisplayDepth)
	static inline constexpr int32_t ST_MAXLIGHTS = 50;

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
		DisplayDepth = 1 << 10,
	};
}



