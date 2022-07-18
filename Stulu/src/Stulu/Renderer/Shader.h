#pragma once
#include <string>
#include <glm/glm.hpp>

namespace Stulu {
#define ST_MAXLIGHTS 50
#define ST_ShaderViewFlags_EnableLighting ((uint32_t)ShaderViewFlags::EnableLighting)
#define ST_ShaderViewFlags_DisplayDiffuse ((uint32_t)ShaderViewFlags::DisplayDiffuse)
#define ST_ShaderViewFlags_DisplaySpecular ((uint32_t)ShaderViewFlags::DisplaySpecular)
#define ST_ShaderViewFlags_DisplayNormal ((uint32_t)ShaderViewFlags::DisplayNormal)
#define ST_ShaderViewFlags_DisplayRoughness ((uint32_t)ShaderViewFlags::DisplayRoughness)
#define ST_ShaderViewFlags_DisplayMetallic ((uint32_t)ShaderViewFlags::DisplayMetallic)
#define ST_ShaderViewFlags_DisplayAmbient ((uint32_t)ShaderViewFlags::DisplayAmbient)
#define ST_ShaderViewFlags_DisplayTexCoords ((uint32_t)ShaderViewFlags::DisplayTexCoords)
#define ST_ShaderViewFlags_DisplayVertices ((uint32_t)ShaderViewFlags::DisplayVertices)
	enum class ShaderViewFlags {
		EnableLighting = 1 << 0,
		//the following will only display itself(DisplayNormal will only display normal map)
		DisplayDiffuse = 1 << 1,
		DisplaySpecular = 1 << 2,
		DisplayNormal = 1 << 3,
		DisplayRoughness = 1 << 4,
		DisplayMetallic = 1 << 5,
		DisplayAmbient = 1 << 6,
		DisplayTexCoords = 1 << 7,
		DisplayVertices = 1 << 8,
	};

	class STULU_API ShaderProperity {
	public:
		enum class Type{
			None,Color4,Range,Enum,Marker
		};
		virtual Type getType() const = 0;

		static Type typeFromString(const std::string& str);
	};

	class STULU_API Shader {
	public:
		virtual ~Shader() = default;

		virtual void bind() const = 0;
		virtual void unbind() const = 0;
		virtual const std::string& getName() const = 0;
		virtual const std::string& getSource(bool afterPreProcessing = true) const = 0;

		static Ref<Shader> create(const std::string& path);
		static Ref<Shader> create(const std::string& name, const std::string& src);
		static Ref<Shader> create(const std::string& name, const std::string& vertex, const std::string& fragment);

		virtual void reload(const std::string& path) = 0;

		virtual void setMat4(const std::string& name, const glm::mat4& mat) = 0;
		virtual void setFloat4(const std::string& name, const glm::vec4& vec) = 0;
		virtual void setFloat3(const std::string& name, const glm::vec3& vec) = 0;
		virtual void setFloat2(const std::string& name, const glm::vec2& vec) = 0;
		virtual void setFloat(const std::string& name, const float Float) = 0;
		virtual void setInt(const std::string& name, const int32_t Int) = 0;
		virtual void setIntArray(const std::string& name, const int* values, uint32_t count) = 0;
		
		//proprityName, properity
		virtual std::unordered_map<std::string, Ref<ShaderProperity>> getProperitys() = 0;
		virtual Ref<ShaderProperity> getProperity(std::string properityName) = 0;
		virtual bool hasProperity(std::string properityName) = 0;

	protected:
		const static std::vector<std::pair<std::string, std::string>> s_preProcessorAdds;
	};

	class STULU_API ShaderLibary {
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

	class STULU_API ShaderProperityRange : public ShaderProperity {
	public:
		ShaderProperityRange(const std::string& values);
		ShaderProperityRange(const float min, const float max)
			: m_min(min), m_max(max) {}

		float getMin() { return m_min; }
		float getMax() { return m_max; }

		virtual Type getType() const override { return Type::Range; }
	private:
		float m_min = .01f, m_max = 1.0f;
	};
	class STULU_API ShaderProperityColor4 : public ShaderProperity {
	public:
		ShaderProperityColor4() {}
		virtual Type getType() const override { return Type::Color4; }
	};
	class STULU_API ShaderProperityEnum : public ShaderProperity {
	public:
		ShaderProperityEnum(const std::string& values);
		ShaderProperityEnum(const std::vector<std::string>& names)
			: m_names(names) {}

		const std::vector<std::string>& getNames() { return m_names; }

		virtual Type getType() const override { return Type::Enum; }
	private:
		std::vector<std::string> m_names;
	};
	class STULU_API ShaderProperityMarker : public ShaderProperity {
	public:
		ShaderProperityMarker(const std::string& text);

		const std::string& getText() { return m_text; }

		virtual Type getType() const override { return Type::Marker; }
	private:
		std::string m_text;
	};
}