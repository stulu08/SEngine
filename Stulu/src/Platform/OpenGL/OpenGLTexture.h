#pragma once
#include "Stulu/Renderer/Texture.h"

namespace Stulu {
	class OpenGLTexture2D : public Texture2D {
	public:
		OpenGLTexture2D(const std::string& path);
		virtual ~OpenGLTexture2D();
		virtual void bind(uint32_t slot) const override;
		virtual uint32_t getWidth() const override { return m_width; }
		virtual uint32_t getHeight() const override { return m_height; }
	private:
		std::string m_path;
		uint32_t m_width;
		uint32_t m_height;
		uint32_t m_rendererID;
	};
	class OpenGLCubeMap : public CubeMap {
	public:
		OpenGLCubeMap(const std::vector<std::string>& faces);
		virtual ~OpenGLCubeMap();
		virtual void bind(uint32_t slot) const override;
		virtual uint32_t getWidth() const override { return m_width; }
		virtual uint32_t getHeight() const override { return m_height; }
	private:
		uint32_t m_width;
		uint32_t m_height;
		uint32_t m_rendererID;
	};
}

