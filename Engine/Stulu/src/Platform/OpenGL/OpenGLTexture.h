#pragma once
#include "Stulu/Renderer/Texture.h"

namespace Stulu {
	//internal format, data format
	STULU_API std::pair<uint32_t, uint32_t> TextureFormatToGLenum(TextureFormat format);
	STULU_API uint32_t TextureWrapToGLenum(TextureWrap wrap);
	STULU_API uint32_t TextureFilteringToGLenumMinification(TextureFiltering filter);
	STULU_API uint32_t TextureFilteringToGLenumMagnification(TextureFiltering filter);
	STULU_API bool isCorrectFormat(TextureFormat& format, int channels, const char* flName);
	STULU_API bool isGLTextureFormatFloat(const TextureFormat& format);
	STULU_API bool isGLTextureFormat32BIT(const TextureFormat& format);
	STULU_API bool isGLTextureFormat16BIT(const TextureFormat& format);
	STULU_API bool isTextureFileFloat(const char* flName);

	class STULU_API OpenGLTexture2D : public virtual Texture2D {
	public:
		// Create from file
		OpenGLTexture2D(const std::string& path, const TextureSettings& settings);
		// Create empty
		OpenGLTexture2D(uint32_t width, uint32_t height, const TextureSettings& settings, MSAASamples samples);
		// Create Internal
		OpenGLTexture2D(uint32_t width, uint32_t height, const TextureSettings& settings, MSAASamples samples, uint32_t arrayCount, uint32_t internalID);

		virtual ~OpenGLTexture2D();

		virtual void bind(uint32_t slot) const override;

		virtual void* getNativeRendererObject() const override { return (void*)(&m_rendererID); }

		virtual uint32_t getWidth() const override { return m_width; }
		virtual uint32_t getHeight() const override { return m_height; }
		virtual uint32_t getMipWidth(uint32_t level) const override { return uint32_t((float)m_width / (glm::pow(2, level))); }
		virtual uint32_t getMipHeight(uint32_t level) const override { return uint32_t((float)m_height / (glm::pow(2, level))); }
		virtual MSAASamples GetSamples() const override { return m_sampels; }
		virtual void SetSamples(MSAASamples sampels) override { m_sampels = sampels; }
		virtual uint32_t GetArraySize() const override { return m_settings.arraySize; }

		virtual TextureSettings& getSettings() override { return m_settings; }

		virtual void setData(const void* data, uint32_t size, uint32_t mipLevel = 0) override;
		virtual void setPixel(uint32_t hexData, uint32_t posX, uint32_t posY, uint32_t mipLevel = 0) override;

		virtual void getData(void* data, uint32_t size, uint32_t mipLevel = 0) const override;
		virtual uint32_t getPixel(uint32_t posX, uint32_t posY, uint32_t mipLevel = 0) const override;

		virtual void updateParameters() override;

		virtual bool operator == (const Texture& other) const override;
		virtual operator int() override { return m_rendererID; }

		// internal
		void setSettings(const TextureSettings& settings) { m_settings = settings; }

		uint32_t GetInternalTextureType() const;
	private:
		inline bool IsArray() const { return GetArraySize() > 1; }
		inline bool HasMips() const { return m_settings.levels > 1 || m_settings.filtering == TextureFiltering::Trilinear; }
		inline bool HasMSAA() const { return ((uint32_t)m_sampels) > 1; }

		void CreateTextureResource();
		void GenerateMips();

		MSAASamples m_sampels;
		uint32_t m_width;
		uint32_t m_height;
		uint32_t m_rendererID;
		TextureSettings m_settings;
	};
}

