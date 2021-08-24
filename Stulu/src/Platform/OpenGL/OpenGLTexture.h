#pragma once
#include "Stulu/Renderer/Texture.h"

namespace Stulu {
	class OpenGLTexture2D : public Texture2D {
	public:
		OpenGLTexture2D(uint32_t width, uint32_t height);
		OpenGLTexture2D(const std::string& path);
		virtual ~OpenGLTexture2D();
		virtual void bind(uint32_t slot) const override;
		virtual uint32_t getRendererID() const override;
		virtual uint32_t getWidth() const override { return m_width; }
		virtual uint32_t getHeight() const override { return m_height; }
		virtual void setData(void* data, uint32_t size) const override;
		virtual bool operator == (const Texture& other) const override;
	private:
		std::string m_path;
		uint32_t m_width;
		uint32_t m_height;
		uint32_t m_rendererID;
		uint32_t m_dataFormat;
	};
	class OpenGLCubeMap : public CubeMap {
	public:
		OpenGLCubeMap(const std::vector<std::string>& faces);
		virtual ~OpenGLCubeMap();
		virtual void bind(uint32_t slot) const override;
		virtual uint32_t getRendererID() const override;
		virtual uint32_t getWidth() const override { return m_width; }
		virtual uint32_t getHeight() const override { return m_height; }
		virtual bool operator == (const Texture& other) const override;
	private:
		uint32_t m_width;
		uint32_t m_height;
		uint32_t m_rendererID;
	};
}

