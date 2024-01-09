#pragma once
#include "Stulu/Renderer/Texture.h"

namespace Stulu {
	//internal format, data format
	STULU_API std::pair<uint32_t, uint32_t> TextureFormatToGLenum(TextureFormat& format, int channels = 4);
	STULU_API uint32_t TextureWrapToGLenum(TextureWrap wrap);
	STULU_API uint32_t TextureFilteringToGLenumMinification(TextureFiltering filter);
	STULU_API uint32_t TextureFilteringToGLenumMagnification(TextureFiltering filter);
	STULU_API bool isCorrectFormat(TextureFormat& format, int channels, const char* flName);
	STULU_API bool isGLTextureFormatFloat(const TextureFormat& format);
	STULU_API bool isGLTextureFormat32BIT(const TextureFormat& format);
	STULU_API bool isGLTextureFormat16BIT(const TextureFormat& format);
	STULU_API bool isTextureFileFloat(const char* flName);

	class STULU_API OpenGLTexture2D : public Texture2D {
	public:
		OpenGLTexture2D(uint32_t internalID, uint32_t width, uint32_t height, const TextureSettings& settings);

		OpenGLTexture2D(uint32_t width, uint32_t height, const TextureSettings& settings);
		OpenGLTexture2D(const std::string& path, const TextureSettings& settings = TextureSettings());
		virtual ~OpenGLTexture2D();

		virtual void bind(uint32_t slot) const override;

		virtual void* getNativeRendererObject() const override { return (void*)(&m_rendererID); }

		virtual uint32_t getWidth() const override { return m_width; }
		virtual uint32_t getHeight() const override { return m_height; }
		virtual uint32_t getMipWidth(uint32_t level) const override { return uint32_t((float)m_width / (glm::pow(2, level))); }
		virtual uint32_t getMipHeight(uint32_t level) const override { return uint32_t((float)m_height / (glm::pow(2, level))); }

		virtual TextureSettings& getSettings() override { return m_settings; }

		virtual void setData(const void* data, uint32_t size, uint32_t mipLevel = 0) override;
		virtual void setPixel(uint32_t hexData, uint32_t posX, uint32_t posY, uint32_t mipLevel = 0) override;

		virtual void getData(void* data, uint32_t size, uint32_t mipLevel = 0) const override;
		virtual uint32_t getPixel(uint32_t posX, uint32_t posY, uint32_t mipLevel = 0) const override;

		virtual void update() override;
		virtual void updateParameters() override;

		virtual bool operator == (const Texture& other) const override;
		virtual operator int() override { return m_rendererID; }

		//temp
		virtual std::string getPath() const override { return m_path; }

		// internal
		void setSettings(const TextureSettings& settings) { m_settings = settings; }
	private:
		inline bool HasMips() const { return m_settings.levels > 1 || m_settings.filtering == TextureFiltering::Trilinear; }

		std::string m_path = "";
		uint32_t m_width;
		uint32_t m_height;
		uint32_t m_rendererID;
		TextureSettings m_settings;
	};
}

