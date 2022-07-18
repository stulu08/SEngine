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

		TextureSettings::Format form = (TextureSettings::Format)settings.format;
		std::pair<GLenum, GLenum> format = TextureFormatToGLenum(form);
		m_dataFormat = format.second;
	}
	OpenGLTexture2D::OpenGLTexture2D(uint32_t width, uint32_t height) {
		ST_PROFILING_FUNCTION();
		m_width = width;
		m_height = height;
		m_settings = TextureSettings();
		GLenum internalFormat = GL_RGBA8;
		m_dataFormat = GL_RGBA;

		glCreateTextures(GL_TEXTURE_2D, 1, &m_rendererID);
		glTextureStorage2D(m_rendererID, 1, internalFormat, m_width, m_height);

		glTextureParameteri(m_rendererID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTextureParameteri(m_rendererID, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		glTextureParameteri(m_rendererID, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTextureParameteri(m_rendererID, GL_TEXTURE_WRAP_T, GL_REPEAT);
	}
	OpenGLTexture2D::OpenGLTexture2D(const std::string& path, const TextureSettings& settings)
		:m_path(path), m_settings(settings) {
		ST_PROFILING_FUNCTION();
		glCreateTextures(GL_TEXTURE_2D, 1, &m_rendererID);
		update();
	}
	OpenGLTexture2D::~OpenGLTexture2D() {
		ST_PROFILING_FUNCTION();
		glDeleteTextures(1, &m_rendererID);
	}
	void OpenGLTexture2D::bind(uint32_t slot) const {
		ST_PROFILING_FUNCTION();
		glBindTextureUnit(slot, m_rendererID);
	}
	void OpenGLTexture2D::setData(void* data, uint32_t size) {
		ST_PROFILING_FUNCTION();

		uint32_t pixelSize = m_dataFormat == GL_RGBA ? 4 : 3;
		CORE_ASSERT(size == m_width * m_height * pixelSize, "Data is not entire Texture");
		//GL_UNSIGNED_INT_8_8_8_8 for a hex value like 0xffff00ff
		glTextureSubImage2D(m_rendererID, 0, 0, 0, m_width, m_height, m_dataFormat, GL_UNSIGNED_INT_8_8_8_8, data);
	}
	void OpenGLTexture2D::setPixel(uint32_t hexData, uint32_t posX, uint32_t posY) {
		//GL_UNSIGNED_INT_8_8_8_8 for a hex value like 0xffff00ff
		glTextureSubImage2D(m_rendererID, 0, posX, posY, 1, 1, m_dataFormat, GL_UNSIGNED_INT_8_8_8_8, &hexData);
	}
	void OpenGLTexture2D::update() {
		ST_PROFILING_FUNCTION();

		int32_t width, height, channels;
		stbi_set_flip_vertically_on_load(1);
		stbi_uc* textureData = nullptr;
		float* floatData = nullptr;
		{
			ST_PROFILING_SCOPE("reading file - OpenGLTexture2D::OpenGLTexture2D(const std::string&)");
			if (isGLTextureFormatFloat((TextureSettings::Format)m_settings.format)) {
				floatData = stbi_loadf(m_path.c_str(), &width, &height, &channels, 0);
				CORE_ASSERT(floatData, "Texture failed to load: {0}", m_path);
			}
			else {
				textureData = stbi_load(m_path.c_str(), &width, &height, &channels, 0);
				CORE_ASSERT(textureData, "Texture failed to load: {0}", m_path);
			}
		}
		m_width = width;
		m_height = height;

		std::pair<GLenum, GLenum> format = TextureFormatToGLenum((TextureSettings::Format&)m_settings.format, channels);
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
		
		glTextureStorage2D(m_rendererID, 1, internalFormat, m_width, m_height);

		glTextureParameteri(m_rendererID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTextureParameteri(m_rendererID, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		glTextureParameteri(m_rendererID, GL_TEXTURE_WRAP_S, wrap);
		glTextureParameteri(m_rendererID, GL_TEXTURE_WRAP_T, wrap);

		if(floatData)
			glTextureSubImage2D(m_rendererID, 0, 0, 0, m_width, m_height, m_dataFormat, GL_FLAT, floatData);
		else
			glTextureSubImage2D(m_rendererID, 0, 0, 0, m_width, m_height, m_dataFormat, GL_UNSIGNED_BYTE, textureData);

		stbi_image_free(textureData);
	}

	uint32_t OpenGLTexture2D::getRendererID() const {
		return m_rendererID;
	}
	bool OpenGLTexture2D::operator==(const Texture& other) const {
		return m_rendererID == other.getRendererID();
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
		return format == TextureSettings::Format::RGB16F || format == TextureSettings::Format::RGBA16F;
	}
}