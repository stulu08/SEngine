#pragma once
#include "Stulu/Core/UUID.h"

namespace Stulu {
	struct TextureSettings {
		enum class Format {
			RGBA, RGB, RG, A, SRGB, SRGBA, RGBA16F, RGBA32F, RGB16F, Auto
		};
		int format = (int)Format::Auto;
		enum class Wrap {
			Clamp, Repeat,
		};
		int wrap = (int)Wrap::Repeat;
		glm::vec2 tiling = { 1.0f,1.0f };
		uint32_t levels = 1;
		TextureSettings(Format format = Format::Auto, Wrap wrap = Wrap::Repeat, const glm::vec2& tiling = { 1.0f,1.0f }, uint32_t levels = 1)
			:format((int)format), wrap((int)wrap), tiling(tiling), levels(levels) {}
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
		virtual void setData(void* data, uint32_t size, uint32_t mipLevel = 0) = 0;
		virtual void setPixel(uint32_t hexData, uint32_t posX, uint32_t posY, uint32_t mipLevel = 0) = 0;
		virtual void update() = 0;

		virtual uint32_t getMipWidth(uint32_t level) const = 0;
		virtual uint32_t getMipHeight(uint32_t level) const = 0;

		virtual std::string getPath() const = 0;
	};
	class STULU_API CubeMap : public Texture {
	public:
		static Ref<CubeMap> create(uint32_t resolution, TextureSettings settings = TextureSettings());
		virtual void* getMap() const = 0;

		virtual void draw() const = 0;
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