#pragma once
#include <string>
#include <glm/glm.hpp>

namespace Stulu {
	class STULU_API ShaderProperity {
	public:
		enum class Type{
			None,Color4,Range,Enum
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
}