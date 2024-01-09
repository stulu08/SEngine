#pragma once
#include "Stulu/Core/UUID.h"
namespace Stulu {
	static inline constexpr int32_t ST_MAX_REFLECTION_LOD = 4;
	static inline constexpr int32_t ST_REFLECTION_MAP_SIZE = 512;

	enum class TextureFormat : uint32_t {
		RGBA = 0, RGB = 1, RG = 2, R = 3, 
		SRGB = 4, SRGBA = 5, 
		RGBA16F = 6, RGB16F = 7, RG16F = 21, R16F = 22,
		RGBA32F = 9, RGB32F = 19, RG32F = 8, R32F = 20,
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
	struct TextureSettings {
		TextureFormat format = TextureFormat::Auto;
		TextureWrap wrap = TextureWrap::Repeat;
		glm::vec2 tiling = { 1.0f,1.0f };
		uint32_t levels = 1;
		glm::vec4 border = glm::vec4(1.0f);
		TextureFiltering filtering = TextureFiltering::Linear;

		TextureSettings(TextureFormat format = TextureFormat::Auto, TextureWrap wrap = TextureWrap::Repeat, 
			const glm::vec2& tiling = { 1.0f,1.0f }, uint32_t levels = 1, glm::vec4 border = glm::vec4(1.0f),
			TextureFiltering filtering = TextureFiltering::Linear)
			:format(format), wrap(wrap), tiling(tiling), levels(levels), border(border), filtering(filtering) {}
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

		UUID uuid;//I dont like it too
	};
	class STULU_API Texture2D : public Texture {
	public:
		static Ref<Texture2D> create(const std::string& path, const TextureSettings& settings = TextureSettings());
		static Ref<Texture2D> create(uint32_t width, uint32_t height, const TextureSettings& settings = TextureSettings());
		virtual void setData(const void* data, uint32_t size, uint32_t mipLevel = 0) = 0;
		virtual void setPixel(uint32_t hexData, uint32_t posX, uint32_t posY, uint32_t mipLevel = 0) = 0;
		virtual void getData(void* data, uint32_t size, uint32_t mipLevel = 0) const = 0;
		virtual uint32_t getPixel(uint32_t posX, uint32_t posY, uint32_t mipLevel = 0) const = 0;
		virtual void update() = 0;
		//only TextureFiltering,TextureWrap and border color
		virtual void updateParameters() = 0;

		virtual uint32_t getMipWidth(uint32_t level) const = 0;
		virtual uint32_t getMipHeight(uint32_t level) const = 0;

		virtual std::string getPath() const = 0;
	};
	class STULU_API CubeMap : public Texture {
	public:
		static Ref<CubeMap> create(uint32_t resolution, TextureSettings settings = TextureSettings());
		virtual void* getMap() const = 0;

		virtual void draw() const = 0;
		virtual void genMips() const = 0;
	};
	class STULU_API ReflectionMap : public CubeMap {
	public:
		static Ref<ReflectionMap> create(uint32_t resolution, TextureSettings settings = TextureSettings(TextureFormat::RGB16F, TextureWrap::ClampToEdge, { 1,1 }, ST_MAX_REFLECTION_LOD + 1));
		//6 for all sides
		virtual void GenerateReflectionMips(uint32_t side) = 0;
	};
	class STULU_API SkyBox : public CubeMap {
	public:
		static Ref<SkyBox> create(const std::string& path);
		static Ref<SkyBox> create(uint32_t resolution, void* data);
		static Ref<SkyBox> create(const std::vector<std::string>& faces, uint32_t resolution);
		static Ref<SkyBox> create(const std::string& hdrTexturePath, uint32_t resolution);
		static Ref<SkyBox> createYAML(const std::string& cubeMapYamlPath, uint32_t resolution);

		virtual void bindEnviromente(uint32_t slot = 0) const = 0;
		virtual void bindIrradiance(uint32_t slot = 1) const = 0;
		virtual void bindPrefilter(uint32_t slot = 2) const = 0;
		virtual void bindBRDFLUT(uint32_t slot = 3) const = 0;

		void update(const std::string& path);
		void updateYAML(const std::string& cubeMapYamlPath, uint32_t resolution);
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