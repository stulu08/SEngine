#pragma once
#include "Stulu/Renderer/Texture.h"

namespace Stulu {
	//internal format, data format
	std::pair<uint32_t, uint32_t> TextureFormatToGLenum(TextureSettings::Format& format, int channels = 4);
	bool CorrectFormat(TextureSettings::Format format, int channels);
	bool isGLTextureFormatFloat(const TextureSettings::Format& format);

	class STULU_API OpenGLTexture2D : public Texture2D {
	public:
		OpenGLTexture2D(uint32_t openglID, uint32_t width, uint32_t height, const TextureSettings& m_settings);
		OpenGLTexture2D(uint32_t width, uint32_t height, const TextureSettings& settings);
		OpenGLTexture2D(const std::string& path, const TextureSettings& settings = TextureSettings());
		virtual ~OpenGLTexture2D();
		virtual void bind(uint32_t slot) const override;
		virtual void* getNativeRendererObject() const override { return (void*)(&m_rendererID); }
		virtual uint32_t getWidth() const override { return m_width; }
		virtual uint32_t getHeight() const override { return m_height; }
		virtual uint32_t getMipWidth(uint32_t level) const override { return uint32_t((float)m_width / (glm::pow(4, level))); }
		virtual uint32_t getMipHeight(uint32_t level) const override { return uint32_t((float)m_height / (glm::pow(4, level))); }
		virtual TextureSettings& getSettings() override { return m_settings; }
		virtual void setData(void* data, uint32_t size, uint32_t mipLevel = 0) override;
		virtual void setPixel(uint32_t hexData, uint32_t posX, uint32_t posY, uint32_t mipLevel = 0) override;
		virtual void update() override;
		virtual bool operator == (const Texture& other) const override;
		virtual operator int() override { return m_rendererID; }
		//temp
		virtual std::string getPath() const override { return m_path; }
	private:
		std::string m_path;
		uint32_t m_width;
		uint32_t m_height;
		uint32_t m_rendererID;
		uint32_t m_dataFormat;
		bool m_hasMips = false;
		TextureSettings m_settings;
	};
}

