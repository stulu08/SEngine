#include "st_pch.h"
#include "OpenGLTexture.h"

#include <glad/glad.h>
#include <stb_image.h>

namespace Stulu {
	OpenGLTexture2D::OpenGLTexture2D(uint32_t openglID, uint32_t width, uint32_t height, const TextureSettings& settings) {
		m_rendererID = openglID;
		m_width = width;
		m_height = height;
		m_settings = settings;
		m_hasMips = m_settings.levels > 1;
		TextureSettings::Format form = (TextureSettings::Format)settings.format;
		std::pair<GLenum, GLenum> format = TextureFormatToGLenum(form);
		m_dataFormat = format.second;
	}
	OpenGLTexture2D::OpenGLTexture2D(uint32_t width, uint32_t height, const TextureSettings& settings) {
		ST_PROFILING_FUNCTION();
		m_width = width;
		m_height = height;
		m_settings = settings;
		m_hasMips = m_settings.levels > 1;

		TextureSettings::Format form = (TextureSettings::Format)settings.format;
		std::pair<GLenum, GLenum> format = TextureFormatToGLenum(form);
		m_dataFormat = format.second;
		GLenum wrap = 0;
		switch (m_settings.wrap)
		{
		case (int)TextureSettings::Wrap::Clamp:
			wrap = GL_CLAMP;
			break;
		case (int)TextureSettings::Wrap::Repeat:
			wrap = GL_REPEAT;
			break;
		}
		glCreateTextures(GL_TEXTURE_2D, 1, &m_rendererID);
		glTextureStorage2D(m_rendererID, m_settings.levels, format.first, m_width, m_height);

		glTextureParameteri(m_rendererID, GL_TEXTURE_MIN_FILTER, m_hasMips ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR);
		glTextureParameteri(m_rendererID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		
		glTextureParameteri(m_rendererID, GL_TEXTURE_WRAP_S, wrap);
		glTextureParameteri(m_rendererID, GL_TEXTURE_WRAP_T, wrap);

		if (m_hasMips)
			glGenerateMipmap(GL_TEXTURE_2D);
		
	}
	OpenGLTexture2D::OpenGLTexture2D(const std::string& path, const TextureSettings& settings)
		:m_path(path), m_settings(settings) {
		ST_PROFILING_FUNCTION();
		update();
	}
	OpenGLTexture2D::~OpenGLTexture2D() {
		ST_PROFILING_FUNCTION();
		glDeleteTextures(1, &m_rendererID);
	}
	void OpenGLTexture2D::bind(uint32_t slot) const {
		glBindTextureUnit(slot, m_rendererID);
	}
	void OpenGLTexture2D::setData(void* data, uint32_t size, uint32_t mipLevel) {
		ST_PROFILING_FUNCTION();
		
		//make available for other formats
		//uint32_t pixelSize = m_dataFormat == GL_RGBA ? 4 : 3;
		//CORE_ASSERT(size == m_width * m_height * pixelSize, "Data is not entire Texture");

		glTextureSubImage2D(m_rendererID, mipLevel, 0, 0, m_width, m_height, m_dataFormat, GL_UNSIGNED_BYTE, data);
	}
	void OpenGLTexture2D::setPixel(uint32_t hexData, uint32_t posX, uint32_t posY, uint32_t mipLevel) {
		//GL_UNSIGNED_INT_8_8_8_8 for a hex value like 0xffff00ff
		glTextureSubImage2D(m_rendererID, mipLevel, posX, posY, 1, 1, m_dataFormat, GL_UNSIGNED_INT_8_8_8_8, &hexData);
	}
	void OpenGLTexture2D::update() {
		ST_PROFILING_FUNCTION();
		if (m_rendererID)
			glDeleteTextures(1, &m_rendererID);
		m_hasMips = m_settings.levels > 1;
		int32_t width, height, channels;
		stbi_set_flip_vertically_on_load(1);
		void* textureData = nullptr;
		bool float_ = false;
		{
			ST_PROFILING_SCOPE("reading file - OpenGLTexture2D::OpenGLTexture2D(const std::string&)");
			if (isGLTextureFormatFloat((TextureSettings::Format)m_settings.format)) {
				textureData = stbi_loadf(m_path.c_str(), &width, &height, &channels, 0);
				float_ = true;
			}
			else {
				textureData = stbi_load(m_path.c_str(), &width, &height, &channels, 0);
			}
			CORE_ASSERT(textureData, "Texture failed to load: {0}", m_path);
		}
		m_width = width;
		m_height = height;

		TextureSettings::Format form = (TextureSettings::Format)m_settings.format;
		std::pair<GLenum, GLenum> format = TextureFormatToGLenum(form, channels);
		GLenum internalFormat = format.first;
		m_dataFormat = format.second;
		GLenum wrap = 0;
		switch (m_settings.wrap)
		{
		case (int)TextureSettings::Wrap::Clamp:
			wrap = GL_CLAMP;
			break;
		case (int)TextureSettings::Wrap::Repeat:
			wrap = GL_REPEAT;
			break;
		}
		
		glCreateTextures(GL_TEXTURE_2D, 1, &m_rendererID);
		glBindTexture(GL_TEXTURE_2D, m_rendererID);
		glTextureStorage2D(m_rendererID, m_settings.levels, internalFormat, m_width, m_height);

		glTextureParameteri(m_rendererID, GL_TEXTURE_MIN_FILTER, m_hasMips ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR);
		glTextureParameteri(m_rendererID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTextureParameteri(m_rendererID, GL_TEXTURE_WRAP_S, wrap);
		glTextureParameteri(m_rendererID, GL_TEXTURE_WRAP_T, wrap);

		if (textureData) {
			glTextureSubImage2D(m_rendererID, 0, 0, 0, m_width, m_height, m_dataFormat, float_ ? GL_FLOAT : GL_UNSIGNED_BYTE, textureData);
			stbi_image_free(textureData);
		}

		if (m_hasMips)
			glGenerateMipmap(GL_TEXTURE_2D);
	}
	bool OpenGLTexture2D::operator==(const Texture& other) const {
		return m_rendererID == *static_cast<uint32_t*>(other.getNativeRendererObject());
	}
	std::pair<uint32_t, uint32_t> TextureFormatToGLenum(TextureSettings::Format& format, int channels) {
		GLenum internalFormat, m_dataFormat;
		switch (format)
		{
		case TextureSettings::Format::A:
			internalFormat = GL_ALPHA8;
			m_dataFormat = GL_ALPHA;
			break;
		case TextureSettings::Format::RG:
			internalFormat = GL_RG;
			m_dataFormat = GL_RG8;
			break;
		case TextureSettings::Format::RGB:
			internalFormat = GL_RGB8;
			m_dataFormat = GL_RGB;
			break;
		case TextureSettings::Format::RGBA:
			internalFormat = GL_RGBA8;
			m_dataFormat = GL_RGBA;
			break;
		case TextureSettings::Format::SRGB:
			internalFormat = GL_SRGB8;
			m_dataFormat = GL_RGB;
			break;
		case TextureSettings::Format::SRGBA:
			internalFormat = GL_SRGB8_ALPHA8;
			m_dataFormat = GL_RGBA;
			break;
		case TextureSettings::Format::RGB16F:
			internalFormat = GL_RGB16F;
			m_dataFormat = GL_RGB;
			break;
		case TextureSettings::Format::RGBA16F:
			internalFormat = GL_RGBA16F;
			m_dataFormat = GL_RGBA;
			break;
		case TextureSettings::Format::RGBA32F:
			internalFormat = GL_RGBA32F;
			m_dataFormat = GL_RGBA;
			break;
		case TextureSettings::Format::Auto:
			switch (channels) {
			case 4:
				internalFormat = GL_RGBA8;
				m_dataFormat = GL_RGBA;
				format = TextureSettings::Format::RGBA;
				break;
			case 3:
				internalFormat = GL_RGB8;
				m_dataFormat = GL_RGB;
				format = TextureSettings::Format::RGB;
				break;
			case 2:
				internalFormat = GL_RG8;
				m_dataFormat = GL_RG;
				format = TextureSettings::Format::RG;
				break;
			case 1:
				internalFormat = GL_ALPHA8;
				m_dataFormat = GL_ALPHA;
				format = TextureSettings::Format::A;
				break;
			}
			break;
		}
		return { internalFormat,m_dataFormat };
	}
	bool isGLTextureFormatFloat(const TextureSettings::Format& format) {
		return format == TextureSettings::Format::RGB16F || format == TextureSettings::Format::RGBA16F || format == TextureSettings::Format::RGBA32F;
	}
}