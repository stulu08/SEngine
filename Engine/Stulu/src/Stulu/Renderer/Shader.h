#pragma once
#include "Stulu/Renderer/Texture.h"

namespace Stulu {
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

	class STULU_API ShaderProperity {
	public:
		enum class Type{
			None,Color,Range,Enum,Marker
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
		virtual std::unordered_map<std::string, Ref<ShaderProperity>>& getProperitys() = 0;
		virtual Ref<ShaderProperity>& getProperity(std::string properityName) = 0;
		virtual bool hasProperity(std::string properityName) = 0;

		static void procesInlucdes(std::string& source);
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
	//will change this so that the props are needed for every material properity, and not like now, where they only modify how certein ones are displayed in editor
	class STULU_API ShaderProperityRange : public ShaderProperity {
	public:
		ShaderProperityRange(const std::string& values);
		ShaderProperityRange(const float min, const float max)
			: m_min(min), m_max(max) {}

		float getMin() const { return m_min; }
		float getMax() const { return m_max; }

		virtual Type getType() const override { return Type::Range; }
	private:
		float m_min = .01f, m_max = 1.0f;
	};
	class STULU_API ShaderProperityColor : public ShaderProperity {
	public:
		ShaderProperityColor(const std::string& values);
		ShaderProperityColor(bool hdr)
			: m_hdr(hdr) {}

		bool isHDR() const { return m_hdr; }

		virtual Type getType() const override { return Type::Color; }
	private:
		bool m_hdr = false;
	};
	class STULU_API ShaderProperityEnum : public ShaderProperity {
	public:
		ShaderProperityEnum(const std::string& values);
		ShaderProperityEnum(const std::vector<std::string>& names)
			: m_names(names) {}

		const std::vector<std::string>& getNames() const { return m_names; };

		virtual Type getType() const override { return Type::Enum; }
	private:
		std::vector<std::string> m_names;
	};
	class STULU_API ShaderProperityMarker : public ShaderProperity {
	public:
		ShaderProperityMarker(const std::string& text);

		const std::string& getText() const { return m_text; }

		virtual Type getType() const override { return Type::Marker; }
	private:
		std::string m_text;
	};
	enum class AccesMode {
		ReadOnly = 0x88B8,
		WriteOnly = 0x88B9,
		ReadWrite = 0x88BA
	};
	class STULU_API ComputeShader {
	public:
		struct Usage {
			static inline const uint32_t None = 0x00000000;// Wont wait for execution finish
			static inline const uint32_t VertexAttribArray = 0x00000001;
			static inline const uint32_t ElementArray = 0x00000002;
			static inline const uint32_t Uniform = 0x00000004;
			static inline const uint32_t TextureFetch = 0x00000008;
			static inline const uint32_t ShaderImageAcces = 0x00000020;
			static inline const uint32_t Command = 0x00000040;
			static inline const uint32_t PixelBuffer = 0x00000080;
			static inline const uint32_t TextureUpdate = 0x00000100;
			static inline const uint32_t BufferUpdate = 0x00000200;
			static inline const uint32_t Framebuffer = 0x00000400;
			static inline const uint32_t TransformFeedback = 0x00000800;
			static inline const uint32_t AtomicCounter = 0x00001000;
			static inline const uint32_t Default = 0xFFFFFFFF;
		};

		virtual ~ComputeShader() = default;
		
		static Ref<ComputeShader> create(const std::string& name, const std::string& src);

		virtual void reload(const std::string& src) = 0;
		virtual void Dispatch(const glm::uvec3& numWorkGroups = { 1,1,1 }, uint32_t usage = Usage::Default) = 0;
		virtual void Dispatch(const uint32_t numWorkGroupsX = 1, const uint32_t numWorkGroupsY = 1, const uint32_t numWorkGroupsZ = 1, uint32_t usage = Usage::Default) = 0;

		virtual void setTexture(const std::string& name, uint32_t binding, const Ref<Texture>& texture, uint32_t mipLevel = 0, AccesMode mode = AccesMode::ReadWrite) = 0;
		virtual void setTextureInternal(const std::string& name, uint32_t binding, void* texture, uint32_t mipLevel = 0, AccesMode mode = AccesMode::ReadWrite, TextureFormat format = TextureFormat::Auto) = 0;
		virtual void setFloat(const std::string& name, float value) = 0;
		virtual void setInt(const std::string& name, int value) = 0;
		virtual void setVec(const std::string& name, const glm::vec4& value) = 0;
		virtual void setMat(const std::string& name, const glm::mat4& value) = 0;
		virtual void setFloatArray(const std::string& name, const float* floats, uint32_t count) = 0;
		virtual void setIntArray(const std::string& name, const int* ints, uint32_t count) = 0;
		virtual void setVecArray(const std::string& name, const glm::vec4* vecs, uint32_t count) = 0;
		virtual void setMatArray(const std::string& name, const glm::mat4* mats, uint32_t count) = 0;

		virtual const std::string& getName() const = 0;
		virtual const std::string& getSource() const = 0;
		virtual const void* getNativeRendererObject() const = 0;
	};
}