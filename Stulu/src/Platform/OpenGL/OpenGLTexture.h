#pragma once
#include "Stulu/Renderer/Texture.h"

namespace Stulu {
	std::pair<uint32_t, uint32_t> TextureFormatToGLenum(TextureSettings::Format& format, int channels = 4);
	class STULU_API OpenGLTexture2D : public Texture2D {
	public:
		OpenGLTexture2D(uint32_t width, uint32_t height);
		OpenGLTexture2D(const std::string& path, const TextureSettings& settings = TextureSettings());
		virtual ~OpenGLTexture2D();
		virtual void bind(uint32_t slot) const override;
		virtual uint32_t getRendererID() const override;
		virtual uint32_t getWidth() const override { return m_width; }
		virtual uint32_t getHeight() const override { return m_height; }
		virtual TextureSettings& getSettings() override { return m_settings; }
		virtual void setData(void* data, uint32_t size) const override;
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
		TextureSettings m_settings;
	};
	class STULU_API OpenGLCubeMap : public CubeMap {
	public:
		OpenGLCubeMap(uint32_t width, uint32_t height);
		OpenGLCubeMap(const std::vector<std::string>& faces);
		virtual ~OpenGLCubeMap();
		virtual void bind(uint32_t slot) const override;
		virtual uint32_t getRendererID() const override;
		virtual uint32_t getWidth() const override { return m_width; }
		virtual uint32_t getHeight() const override { return m_height; }
		virtual bool operator == (const Texture& other) const override;
		virtual operator int() override { return m_rendererID; }
	private:
		uint32_t m_width;
		uint32_t m_height;
		uint32_t m_rendererID;
	};
}

