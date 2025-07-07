#pragma once
#include "Stulu/Types/UUID.h"
#include "Stulu/Core/Log.h"


namespace Stulu {
	constexpr auto ST_SKYBOX_TEXTURE_BIND_ENV_OFF = 0;
	constexpr auto ST_SKYBOX_TEXTURE_BIND_IRR_OFF = 1;
	constexpr auto ST_SKYBOX_TEXTURE_BIND_PRE_OFF = 2;
	constexpr auto ST_SKYBOX_TEXTURE_BIND_BRD_OFF = 3;

	constexpr auto ST_SKYBOX_TEXTURE_BIND_ENV = 0 + ST_SKYBOX_TEXTURE_BIND_ENV_OFF;
	constexpr auto ST_SKYBOX_TEXTURE_BIND_IRR = 0 + ST_SKYBOX_TEXTURE_BIND_IRR_OFF;
	constexpr auto ST_SKYBOX_TEXTURE_BIND_PRE = 0 + ST_SKYBOX_TEXTURE_BIND_PRE_OFF;
	constexpr auto ST_SKYBOX_TEXTURE_BIND_BRD = 0 + ST_SKYBOX_TEXTURE_BIND_BRD_OFF;
	constexpr auto ST_SHADOW_TEXTURE_BIND_MAP = 4;

	constexpr auto ST_MAX_REFLECTION_LOD = 4;
	constexpr auto ST_REFLECTION_MAP_SIZE = 512;

	enum class TextureFormat : uint32_t {
		RGBA = 0, RGB = 1, RG = 2, R = 3, 
		SRGB = 4, SRGBA = 5, 
		RGBA16F = 6, RGB16F = 7, RG16F = 21, R16F = 22, R32UI = 23,
		RGBA32F = 9, RGB32F = 19, RG32F = 8, R32F = 20,
		// auto can only be used for files
		Auto = 10, None = 11,
		Depth16 = 12, Depth24 = 13, Depth32 = 14, Depth32F = 15, 
		Depth24_Stencil8 = 16, Depth32F_Stencil8 = 17, Stencil8 = 18

		//MAX: 22
	};
	enum class TextureWrap : uint32_t {
		ClampToEdge, Repeat, ClampToBorder
	};
	enum class TextureFiltering : uint32_t {
		Linear,
		Nearest,
		Trilinear,

		Bilinear = Linear,
	};
	
	struct alignas(8) TextureSettings {
		TextureFormat format = TextureFormat::Auto;
		TextureWrap wrap = TextureWrap::Repeat;
		glm::vec2 tiling = { 1.0f,1.0f };
		uint32_t levels = 1;
		glm::vec4 border = glm::vec4(1.0f);
		TextureFiltering filtering = TextureFiltering::Linear;

		TextureSettings(TextureFormat format, TextureWrap wrap = TextureWrap::ClampToEdge,
			const glm::vec2& tiling = { 1.0f,1.0f }, uint32_t levels = 1, glm::vec4 border = glm::vec4(1.0f),
			TextureFiltering filtering = TextureFiltering::Linear)
			:format(format), wrap(wrap), tiling(tiling), levels(levels), border(border), filtering(filtering) {}
	};

	enum class MSAASamples : uint32_t {
		Disabled = 1,
		Two = 2,
		Four = 4,
		Eight = 8
	};

	class STULU_API Texture
	{
	public:
		virtual ~Texture() = default;

		virtual void bind(uint32_t slot = 0) const = 0;
		virtual void* getNativeRendererObject() const = 0;

		virtual uint32_t getWidth() const = 0;
		virtual uint32_t getHeight() const = 0;

		virtual bool operator == (const Texture & other) const = 0;
		virtual operator int() = 0;

		virtual TextureSettings& getSettings() = 0;
	};
	class STULU_API Texture2D : public virtual Texture {
	public:
		static Ref<Texture2D> create(const std::string& path, const TextureSettings& settings = TextureFormat::Auto);
		static Ref<Texture2D> create(uint32_t width, uint32_t height, const TextureSettings& settings = TextureFormat::RGBA, MSAASamples samples = MSAASamples::Disabled);

		virtual void setData(const void* data, uint32_t size, uint32_t mipLevel = 0) = 0;
		virtual void setPixel(uint32_t hexData, uint32_t posX, uint32_t posY, uint32_t mipLevel = 0) = 0;
		virtual void getData(void* data, uint32_t size, uint32_t mipLevel = 0) const = 0;
		virtual uint32_t getPixel(uint32_t posX, uint32_t posY, uint32_t mipLevel = 0) const = 0;
		//only TextureFiltering,TextureWrap and border color
		virtual void updateParameters() = 0;

		virtual uint32_t getMipWidth(uint32_t level) const = 0;
		virtual uint32_t getMipHeight(uint32_t level) const = 0;

		virtual MSAASamples GetSamples() const = 0;
		virtual void SetSamples(MSAASamples sampels) = 0;
	};



	class STULU_API CubeMap : public virtual Texture {
	public:
		static Ref<CubeMap> create(uint32_t resolution, TextureSettings settings = TextureSettings(TextureFormat::RGB));
		virtual void* getMap() const = 0;

		virtual void genMips() const = 0;
	};
	class STULU_API ReflectionMap : public virtual CubeMap {
	public:
		static Ref<ReflectionMap> create(uint32_t resolution, TextureSettings settings = TextureSettings(TextureFormat::RGB16F, TextureWrap::ClampToEdge, { 1,1 }, ST_MAX_REFLECTION_LOD + 1));
		//6 for all sides
		virtual void GenerateReflectionMips(uint32_t side) = 0;
	};
	class STULU_API SkyBox : public virtual CubeMap {
	public:
		static Ref<SkyBox> create(const std::string& path, uint32_t resolution = 2048);
		static Ref<SkyBox> create(const std::vector<std::string>& faces, uint32_t resolution);
		static Ref<SkyBox> createYAML(const std::string& cubeMapYamlPath, uint32_t resolution);

		virtual void bindEnviromente(uint32_t slot = ST_SKYBOX_TEXTURE_BIND_ENV) const = 0;
		virtual void bindIrradiance(uint32_t slot = ST_SKYBOX_TEXTURE_BIND_IRR) const = 0;
		virtual void bindPrefilter(uint32_t slot = ST_SKYBOX_TEXTURE_BIND_PRE) const = 0;
		virtual void bindBRDFLUT(uint32_t slot = ST_SKYBOX_TEXTURE_BIND_BRD) const = 0;

		void UpdateSkybox(const std::string& path, uint32_t resolution = 2048);
		void UpdateSkyboxYAML(const std::string& cubeMapYamlPath, uint32_t resolution);

		virtual void update(uint32_t resolution, void* data) = 0;
		virtual void update(const std::vector<std::string>& faces, uint32_t resolution) = 0;
		virtual void update(const std::string& hdrTexturePath, uint32_t resolution) = 0;

		virtual void* getEnviroment() const = 0;
		virtual void* getIrradianceMap() const = 0;
		virtual void* getPrefilterMap() const = 0;
		virtual void* getBRDFLUT() const = 0;

		//generates a BRDFLUT or returns an already created one if one with the resolution was already created
		static Ref<Texture>& genrateBRDFLUT(uint32_t resolution);
	};
}